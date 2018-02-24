#include <VideoLib/VideoStreamer/VideoStreamer.hpp>
#include <VideoLib/VideoPlayer/VideoPlayer.hpp>
#include <VideoLib/Utility/Frame.hpp>

extern "C"
{
  #include <libavformat/avformat.h>
  #include <libavutil/mathematics.h>
  #include <libavutil/time.h>
  #include <libavutil/imgutils.h>
};

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <thread>

using AVFramePtr = std::unique_ptr<AVFrame, void(*)(AVFrame*)>;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AVFramePtr GenerateYuvFrame(int Height, int Width, int FrameNumber)
{
  AVFramePtr pFrame(
    av_frame_alloc(),
    [] (auto pFrame)
    {
      av_frame_free(&pFrame);
    });

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
vl::Frame GenerateRgbFrame(int Height, int Width, int FrameNumber)
{
  auto pBytes = std::make_unique<std::byte[]>(Height * Width * 3);

  static int LineNumber = 0;

  for (int y = 0; y < Height; y++)
  {
    auto Y = y * Width;

    for (int x = 0; x < Width; x+=3)
    {
      std::byte WhiteOrBlack = static_cast<std::byte>(126u);

      if (LineNumber == x)
      {
        WhiteOrBlack = static_cast<std::byte>(255u);
      }

      pBytes[Y + x] = WhiteOrBlack;
      pBytes[Y + x + 1] = WhiteOrBlack;
      pBytes[Y + x + 2] = WhiteOrBlack;
    }
  }

  if (++LineNumber > Width)
  {
    LineNumber = 0;
  }

  return vl::Frame((1.0 / 30) * 90 * FrameNumber, Width, Height, std::move(pBytes));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{

  int FrameNumber = 0;

  if (argc == 1)
  {
    int Width = 352, Height = 288;

    //vl::VideoStreamer VideoStreamer("rtp://10.18.15.58:9990", Width, Height);
    vl::VideoStreamer VideoStreamer("rtp://127.0.0.1:9990", Width, Height);

    while (FrameNumber < 5000)
    {
      auto pFrame = GenerateYuvFrame(Height, Width, FrameNumber);

      std::cout << "Write frame " << FrameNumber  << " time = " << pFrame->pts << std::endl;

      VideoStreamer.StreamFrame(std::experimental::make_observer<AVFrame>(pFrame.get()));

      FrameNumber++;
    }
  }
  else
  {
    std::unique_ptr<vl::VideoStreamer> pVideoStreamer(nullptr);

    vl::VideoPlayer videoPlayer(argv[1]);

    videoPlayer.GetSignalFrame().Connect(
      [&pVideoStreamer, &FrameNumber] (const std::shared_ptr<const vl::Frame>& pFrame)
      {
        if (!pVideoStreamer)
        {
          pVideoStreamer = std::make_unique<vl::VideoStreamer>(
            "rtp://127.0.0.1:9990",
            pFrame->GetWidth(),
            pFrame->GetHeight());
        }

        pVideoStreamer->StreamFrame(*pFrame);

        std::cout
          << "Write frame " << FrameNumber  << " time = "
          << pFrame->GetTime() << " " << pFrame->GetWidth()
          << "x" << pFrame->GetHeight() << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        FrameNumber++;
      });

    videoPlayer.GetSignalError().Connect(
      [] (const std::string error)
      {
        std::cerr << error << "\n";
        exit(1);
      });

    while (true)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
  }
}
