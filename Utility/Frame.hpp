#pragma once

#include <Images/Image.hpp>
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
        std::unique_ptr<uint8_t[]>&& pData);

    private:

      double mTime;

      dl::image::Image mImage;
  };
}
