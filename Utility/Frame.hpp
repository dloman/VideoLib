#pragma once

#include <DanLib/Images/Image.hpp>
#include <cstddef>
#include <memory>

namespace vl
{
  class Frame
  {
    public:

      Frame(
        const double time,
        const int width,
        const int height,
        std::unique_ptr<std::byte[]>&& pData);

      const double GetTime() const;

      const dl::image::Image& GetImage() const;

    private:

      double mTime;

      dl::image::Image mImage;
  };

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  inline
  const double Frame::GetTime() const
  {
    return mTime;
  }

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  inline
  const dl::image::Image& Frame::GetImage() const
  {
    return mImage;
  }
}
