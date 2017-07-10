#include "Frame.hpp"

using vl::Frame;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Frame::Frame(
  const double time,
  const int width,
  const int height,
  std::unique_ptr<uint8_t[]>&& pData)
: mTime(time),
  mImage(width, height, std::move(pData))
{
}
