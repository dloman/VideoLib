#include <VideoLib/VideoStreamer/VideoStreamer.hpp>

extern "C"
{
  #include <libavformat/avformat.h>
  #include <libavutil/mathematics.h>
  #include <libavutil/time.h>
#include <libavutil/imgutils.h>
};

#include <iostream>
#include <stdexcept>
#include <thread>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AVFrame* GenerateFrame(int Height, int Width, int FrameNumber)
{
  auto pFrame = av_frame_alloc();

  pFrame->format = AV_PIX_FMT_YUV420P;
  pFrame->width = Width;
  pFrame->height = Height;

  auto ret = av_image_alloc(
    pFrame->data,
    pFrame->linesize,
    Width,
    Height,
    AV_PIX_FMT_YUV420P,
    32);

  /* prepare a dummy image */
  /* Y */
  for (int y = 0; y < Height; y++)
  {
      for (int x = 0; x < Width; x++)
      {
          pFrame->data[0][y * pFrame->linesize[0] + x] = x + y + FrameNumber * 3;
      }
  }
  /* Cb and Cr */
  for (int y = 0; y < Height / 2; y++)
  {
      for (int x = 0; x < Width / 2; x++)
      {
          pFrame->data[1][y * pFrame->linesize[1] + x] = 128 + y + FrameNumber * 2;
          pFrame->data[2][y * pFrame->linesize[2] + x] = 64 + x + FrameNumber * 5;
      }
  }
  pFrame->pts = (1.0 / 30) * 90 * FrameNumber;

  std::this_thread::sleep_for(std::chrono::milliseconds(40));

  return pFrame;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main()
{
  int Width = 352, Height = 288;
  vl::VideoStreamer VideoStreamer("rtp://127.0.0.1:9990", Width, Height);

  int FrameNumber = 0;
  while (FrameNumber < 5000)
  {
    auto pFrame = GenerateFrame(Height, Width, FrameNumber);

    std::cout << "Write frame " << FrameNumber << std::endl;

    VideoStreamer.StreamFrame(pFrame);

    ++FrameNumber;
  }
}



