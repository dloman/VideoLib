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

    private:

      double mTime;

      dl::image::Image mImage;
  };
}
