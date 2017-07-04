#pragma once

#include <Signal/Signal.hpp>
#include <Utility/Frame.hpp>

#include <atomic>
#include <memory>
#include <string>
#include <thread>

struct AVCodecContext;
struct AVFormatContext;
struct AVFrame;
struct AVPacket;

namespace vl
{
  class VideoPlayer
  {
    public:

      VideoPlayer(const std::string& filename);

      ~VideoPlayer();

      VideoPlayer(const VideoPlayer&) = delete;
      VideoPlayer& operator = (const VideoPlayer&) = delete;

      using FrameSignal = dl::Signal<vl::Frame>;

      const FrameSignal GetSignalFrame() const;

    private:

      void Start();

      void Initialize();

      void OpenCodecContext();

      void ProcessPackets();

      void DecodePacket(const AVPacket& packet);

      using FramePtr = std::unique_ptr<AVFrame, void(*)(AVFrame*)>;

      void ConvertFromYuvToRgb(FramePtr& pFrame);

    private:

      std::atomic<bool> mIsRunning;

      using FormatContextPtr =
        std::unique_ptr<AVFormatContext, void(*)(AVFormatContext*)>;

      FormatContextPtr mpFormatContext;


      using CodecContextPtr =
        std::unique_ptr<AVCodecContext, void(*)(AVCodecContext*)>;

      CodecContextPtr mpCodecContext;

      std::unique_ptr<std::thread> mpThread;

      std::string mFilename;

      FrameSignal mSignalFrame;

      double mTimeBase;
  };
}
