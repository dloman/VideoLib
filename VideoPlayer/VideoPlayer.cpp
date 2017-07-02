#include "VideoPlayer.hpp"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <experimental/algorithm>
#include <experimental/optional>
#include <experimental/chrono>
//#include <experimental/memory>
#include <memory>

using vl::VideoPlayer;

namespace
{
  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  std::unique_ptr<AVCodecContext, void(*)(AVCodecContext*)> GetFirstVideoStream(
    //const std::experimental::observer_ptr<AVFormatContext> pFormatContext)
    const AVFormatContext* pFormatContext)
  {
    for (int i = 0; i < pFormatContext->nb_streams; ++i)
    {
      if (pFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
      {
        return
        {
          pFormatContext->streams[i]->codec,
          [](AVCodecContext* pCodecContext) { avcodec_close(pCodecContext); }
        };
      }
    }
    throw std::runtime_error("couldn't find video stream");
  }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VideoPlayer::VideoPlayer(const std::string& filename)
  : mIsRunning(true),
    mpThread(std::make_unique<std::thread>([this] { Start(); })),
    mFilename(filename)
{
  av_register_all();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VideoPlayer::Start()
{
  std::unique_ptr<AVFormatContext, void(*)(AVFormatContext*)> pFormatContext(
    nullptr,
    [](AVFormatContext* pFormatContext){avformat_close_input(&pFormatContext);});

  if (avformat_open_input(pFormatContext.get(), mFilename.c_str(), nullptr, 0, nullptr) != 0)
  {
    throw runtime_error("unable to open " + filename);
  }

  if (avformat_find_stream_info(pFormatContext, nullptr) < 0)
  {
    throw runtime_error("unable to find stream info");
  }

  av_dump_format(pFormatContext, 0, filename.c_str(), 0);

  auto pCodecContext = GetFirstVideoStream();
}


