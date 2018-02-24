#pragma once

#include <experimental/memory>
#include <memory>
#include <string>

class AVBitStreamFilterContext;
class AVCodecContext;
class AVFrame;
class AVFormatContext;
class AVOutputFormat;
class AVStream;

namespace vl
{
  class Frame;

  class VideoStreamer
  {
    public:

      VideoStreamer(const std::string& Url, int Width, int Height);

      ~VideoStreamer();

      void StreamFrame(const vl::Frame& image);

      void StreamFrame(std::experimental::observer_ptr<AVFrame> pFrame);

    private:

      void SetCodec();

      AVFormatContext* mpOutputFormatContext;

      AVCodecContext* mpCodecContext;

      AVStream* mpStream;

      size_t mFrameIndex;

  };
}
