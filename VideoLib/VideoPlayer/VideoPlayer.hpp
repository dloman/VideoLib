#pragma once

#include <DanLib/Signal/Signal.hpp>
#include <VideoLib/Utility/Frame.hpp>

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

      using FrameSignal = dl::Signal<const std::shared_ptr<const vl::Frame>&>;

      const FrameSignal& GetSignalFrame() const;

      using ErrorSignal = dl::Signal<const std::string&>;

      const ErrorSignal& GetSignalError() const;

      using EndSignal = dl::Signal<void>;

      const EndSignal& GetSignalVideoOver() const;

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

      ErrorSignal mSignalError;

      EndSignal mSignalVideoOver;

      double mTimeBase;
  };


  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  inline
  const VideoPlayer::FrameSignal& VideoPlayer::GetSignalFrame() const
  {
    return mSignalFrame;
  }

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  inline
  const VideoPlayer::ErrorSignal& VideoPlayer::GetSignalError() const
  {
    return mSignalError;
  }

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  inline
  const VideoPlayer::EndSignal& VideoPlayer::GetSignalVideoOver() const
  {
    return mSignalVideoOver;
  }
}
