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

      double GetTime() const;

      std::shared_ptr<const dl::image::Image> GetImage() const;

    private:

      double mTime;

      const std::shared_ptr<dl::image::Image> mpImage;
  };

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  inline
  double Frame::GetTime() const
  {
    return mTime;
  }

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  inline
  std::shared_ptr<const dl::image::Image> Frame::GetImage() const
  {
    return mpImage;
  }
}
