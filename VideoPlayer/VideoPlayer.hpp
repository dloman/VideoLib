#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <thread>

namespace vl
{
  class VideoPlayer
  {
    public:

      VideoPlayer(const std::string& filename);

      ~VideoPlayer();

      VideoPlayer(const VideoPlayer&) = delete;
      VideoPlayer& operator = (const VideoPlayer&) = delete;

    private:

      void Start();

    private:

      std::atomic<bool> mIsRunning;

      std::unique_ptr<std::thread> mpThread;

      std::string mFilename;
  };
}
