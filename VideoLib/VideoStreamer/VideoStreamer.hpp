#pragma once

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

      void StreamFrame(AVFrame* pFrame);

    private:

      void SetCodec();

      AVFormatContext* mpOutputFormatContext;

      AVCodecContext* mpCodecContext;

      AVStream* mpStream;

      size_t mFrameIndex;

  };
}
