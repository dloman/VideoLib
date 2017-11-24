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
  class VideoStreamer
  {
    public:

      VideoStreamer(const std::string& Url, int Width, int Height);

      ~VideoStreamer();

      void StreamFrame(AVFrame* pFrame);

    private:

      void SetCodec();

      AVFormatContext* mpOutputFormatContext;

      AVCodecContext* mpCodecContext;

      AVBitStreamFilterContext* mpAnnexB;

      AVOutputFormat* mpOutputFormat;

      AVStream* mpStream;

      size_t mFrameIndex;

  };
}
