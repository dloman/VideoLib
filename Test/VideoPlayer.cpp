#include "VideoPlayer/VideoPlayer.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
  if (argc !=2)
  {
    std::cerr << "VideoPlayer FILENAME\n";
    exit(1);
  }

  vl::VideoPlayer videoPlayer(argv[1]);

  videoPlayer.GetSignalFrame().Connect(
    [] (const vl::Frame& frame)
    {
      std::cout << frame.GetTime() << std::endl;

      const auto& image = frame.GetImage();

      cv::Mat displayImage(
          image.GetHeight(),
          image.GetWidth(),
          CV_8UC3,
          image.GetData().get());

      cv::cvtColor(displayImage, displayImage, CV_BGR2RGB);

      cv::imshow("video player", displayImage);

      cv::waitKey(1);
    });

  videoPlayer.GetSignalError().Connect(
    [] (const std::string error)
    {
      std::cerr << error << "\n";
      exit(1);
    });

  videoPlayer.GetSignalError().Connect([](auto t) { exit(0); });

  using namespace std::literals;

  while (true)
  {
    std::this_thread::sleep_for(250ms);
  }
}
