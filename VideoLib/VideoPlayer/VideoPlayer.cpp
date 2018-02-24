#include "VideoPlayer.hpp"

extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavdevice/avdevice.h>
  #include <libavformat/avformat.h>
  #include <libavutil/error.h>
  #include <libavutil/imgutils.h>
  #include <libswscale/swscale.h>
#include <libavutil/timestamp.h>
}

#include <experimental/memory>
#include <memory>
#include <utility>
#include <type_traits>

using vl::VideoPlayer;

namespace
{
  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  double GetTimeBase(AVStream& stream)
  {
    return av_q2d(stream.time_base);
  }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VideoPlayer::VideoPlayer(const std::string& filename)
  : mIsRunning(true),
    mpFormatContext(nullptr, [] (auto pFormatContext) {}),
    mpCodecContext(nullptr, [] (auto pCodecContext) {}),
    mpThread(std::make_unique<std::thread>([this] { Start(); })),
    mFilename(filename),
    mTimeBase(0.0),
    mVideoStreamIndex(0)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VideoPlayer::~VideoPlayer()
{
  mIsRunning = false;

  if (mpThread && mpThread->joinable())
  {
    mpThread->join();
  }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VideoPlayer::Start()
{
  Initialize();

  ProcessPackets();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VideoPlayer::Initialize()
{
  avdevice_register_all();
  avcodec_register_all();
  av_register_all();


  AVFormatContext* pTempFormatContext = nullptr;

  AVInputFormat* pInputFormat = nullptr;

  if (mFilename.find("/dev/video") != std::string::npos)
  {
    pInputFormat = av_find_input_format("v4l2");
  }

  if (avformat_open_input(&pTempFormatContext, mFilename.c_str(), pInputFormat, nullptr) != 0)
  {
    mSignalError("unable to open " + mFilename);

    return;
  }

  mpFormatContext = std::unique_ptr<AVFormatContext, void(*)(AVFormatContext*)>(
    pTempFormatContext,
    [](AVFormatContext* pFormatContext){avformat_close_input(&pFormatContext);});

  if (avformat_find_stream_info(mpFormatContext.get(), nullptr) < 0)
  {
    mSignalError("unable to find stream info");

    return;
  }

  OpenCodecContext();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VideoPlayer::OpenCodecContext()
{
  AVCodec* pCodec(nullptr);

  AVStream* pStream(nullptr);

  mVideoStreamIndex = av_find_best_stream(
    mpFormatContext.get(),
    AVMEDIA_TYPE_VIDEO,
    -1,
    -1,
    &pCodec,
    0);

  if (mVideoStreamIndex < 0)
  {
    mSignalError("could not find best stream");

    return;
  }

  pStream = mpFormatContext->streams[mVideoStreamIndex];

  mTimeBase = GetTimeBase(*pStream);

  if (!pCodec)
  {
    mSignalError("unable to find codec");

    return;
  }

  mpCodecContext = CodecContextPtr(
    avcodec_alloc_context3(pCodec),
    [] (auto pCodecContext) {avcodec_free_context(&pCodecContext);});

  if (!mpCodecContext)
  {
    mSignalError("unable to alloc codec context");

    return;
  }

  if (avcodec_parameters_to_context(mpCodecContext.get(), pStream->codecpar) < 0)
  {
    mSignalError("unable to copy codec parameters");

    return;
  }

  if (avcodec_open2(mpCodecContext.get(), pCodec, nullptr) < 0)
  {
    mSignalError("unable to open codec");

    return;
  }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VideoPlayer::ProcessPackets()
{
  AVPacket packet;

  while (mIsRunning && av_read_frame(mpFormatContext.get(), &packet) >= 0)
  {
    if (mVideoStreamIndex == packet.stream_index)
    {
      DecodePacket(packet);
    }

    av_packet_unref(&packet);
  }

  mIsRunning = false;

  mSignalVideoOver();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VideoPlayer::DecodePacket(const AVPacket& packet)
{
  auto returnCode = avcodec_send_packet(mpCodecContext.get(), &packet);

  if (returnCode < 0)
  {
    std::array<char, 2056> error;
    av_make_error_string(error.data(), 2056, returnCode);
    mSignalError(std::string("decode error ") + error.data());
  }

  FramePtr pYuvFrame(
    av_frame_alloc(),
    [] (auto pFrame) { av_frame_free(&pFrame); });

  if (!pYuvFrame)
  {
    mSignalError("could not allocate frame");

    return;
  }

  returnCode = avcodec_receive_frame(mpCodecContext.get(), pYuvFrame.get());

  if (returnCode == AVERROR(EAGAIN) || returnCode == AVERROR_EOF)
  {
    return;
  }
  else if (returnCode < 0)
  {
    mSignalError("Error while decoding");
  }

  if (packet.dts == AV_NOPTS_VALUE)
  {
    mSignalError("invaild time");
  }

  ConvertFromYuvToRgb(pYuvFrame);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VideoPlayer::ConvertFromYuvToRgb(FramePtr& pYuvFrame)
{
  //get a context to switch to rgb
  std::unique_ptr<SwsContext, void(*)(SwsContext*)> pSws(
    sws_getContext(
      mpCodecContext->width,
      mpCodecContext->height,
      mpCodecContext->pix_fmt,
      mpCodecContext->width,
      mpCodecContext->height,
      AV_PIX_FMT_RGB24,
      SWS_BILINEAR,
      nullptr,
      nullptr,
      nullptr),
    [] (auto pSwsContext) { sws_freeContext(pSwsContext);});

  if (!pSws)
  {
    mSignalError("unable to get swsContext");
  }

  auto byteCount = av_image_get_buffer_size(
    AV_PIX_FMT_RGB24,
    pYuvFrame->width,
    pYuvFrame->height,
    1);

  std::unique_ptr<std::byte[]> pBytes = std::make_unique<std::byte[]>(byteCount);

  FramePtr pRgbFrame(
    av_frame_alloc(),
    [] (auto pFrame) { av_frame_free(&pFrame); });

  avpicture_fill(
    reinterpret_cast<AVPicture*>(pRgbFrame.get()),
    reinterpret_cast<std::uint8_t*>(pBytes.get()),
    AV_PIX_FMT_RGB24,
    mpCodecContext->width,
    mpCodecContext->height);

  sws_scale(
    pSws.get(),
    pYuvFrame->data,
    pYuvFrame->linesize,
    0,
    mpCodecContext.get()->height,
    pRgbFrame->data,
    pRgbFrame->linesize);

  mSignalFrame(std::make_shared<vl::Frame>(
    av_frame_get_best_effort_timestamp(pYuvFrame.get()) * mTimeBase,
    mpCodecContext->width,
    mpCodecContext->height,
    std::move(pBytes)));
}
