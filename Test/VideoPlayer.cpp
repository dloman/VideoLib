#include <VideoLib/VideoPlayer/VideoPlayer.hpp>

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
    [] (const std::shared_ptr<const vl::Frame>& pFrame)
    {
      std::cout << pFrame->GetTime() << std::endl;

      const auto& pImage = pFrame->GetImage();

      cv::Mat displayImage(
          pImage->GetHeight(),
          pImage->GetWidth(),
          CV_8UC3,
          pImage->GetData().get());

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
