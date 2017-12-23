#pragma once

extern "C"
{
  #include <libavformat/avformat.h>
}

namespace vl
{
  class Packet
  {
    public:

      Packet();

      ~Packet();

      const AVPacket& Get() const;

      AVPacket& Get();

    private:

      AVPacket mPacket;
  };
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline
const AVPacket& vl::Packet::Get() const
{
  return mPacket;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline
AVPacket& vl::Packet::Get()
{
  return mPacket;
}
