#include "VideoStreamer.hpp"

#include <VideoLib/Utility/Packet.hpp>

#include <iostream>
#include <stdexcept>

extern "C"
{
  #include <libavformat/avformat.h>
  #include <libavutil/mathematics.h>
  #include <libavutil/time.h>
  #include <libavutil/opt.h>
}

using vl::VideoStreamer;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
VideoStreamer::VideoStreamer(const std::string& Url, int Width, int Height)
: mpOutputFormatContext(nullptr),
  mpCodecContext(nullptr),
  mpStream(nullptr),
  mFrameIndex(0u)
{
  avcodec_register_all();
  av_register_all();
  avformat_network_init();

  auto pCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
  mpCodecContext = avcodec_alloc_context3(pCodec);

  mpCodecContext->bit_rate = 400000;
  mpCodecContext->width = Width;
  mpCodecContext->height = Height;
  mpCodecContext->time_base.num = 1;
  mpCodecContext->time_base.den = 25;
  mpCodecContext->gop_size = 25;
  mpCodecContext->max_b_frames = 1;
  mpCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
  mpCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;

  av_opt_set(mpCodecContext->priv_data, "preset", "ultrafast", 0);
  av_opt_set(mpCodecContext->priv_data, "tune", "zerolatency", 0);

  avcodec_open2(mpCodecContext, pCodec, nullptr);

  AVOutputFormat* pFormat = av_guess_format("rtp", nullptr, nullptr);

  auto ret = avformat_alloc_output_context2(
    &mpOutputFormatContext,
    pFormat,
    pFormat->name,
    Url.c_str());

  printf("Writing to %s\n", mpOutputFormatContext->filename);

  avio_open(
    &mpOutputFormatContext->pb,
    mpOutputFormatContext->filename,
    AVIO_FLAG_WRITE);

  mpStream = avformat_new_stream(mpOutputFormatContext, pCodec);
  mpStream->codecpar->bit_rate = 400000;
  mpStream->codecpar->width = Width;
  mpStream->codecpar->height = Height;
  mpStream->codecpar->codec_id = AV_CODEC_ID_H264;
  mpStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
  mpStream->time_base.num = 1;
  mpStream->time_base.den = 25;

  avformat_write_header(mpOutputFormatContext, nullptr);
  char buf[200000];
  AVFormatContext *ac[] = { mpOutputFormatContext };
  av_sdp_create(ac, 1, buf, 20000);
  printf("sdp:\n%s\n", buf);
  auto fsdp = fopen("test.sdp", "w");
  fprintf(fsdp, "%s", buf);
  fclose(fsdp);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void VideoStreamer::StreamFrame(AVFrame* pFrame)
{
  vl::Packet Packet;

  auto ret = avcodec_send_frame(mpCodecContext, pFrame);

  if (ret != 0)
  {
    char Error[256];

    av_make_error_string(Error, 256, ret);

    std::cerr << Error << "\n";
  }

  ret = avcodec_receive_packet(mpCodecContext, &Packet.Get());

  if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
  {
    char Error[256];

    av_make_error_string(Error, 256, ret);

    std::cerr << Error << "\n";
    return;
  }

  av_packet_rescale_ts(&Packet.Get(), mpCodecContext->time_base, mpStream->time_base);
  av_interleaved_write_frame(mpOutputFormatContext, &Packet.Get());

  av_freep(&pFrame->data[0]);
  av_frame_free(&pFrame);

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
VideoStreamer::~VideoStreamer()
{
  avcodec_close(mpCodecContext);
  av_free(mpCodecContext);
}
