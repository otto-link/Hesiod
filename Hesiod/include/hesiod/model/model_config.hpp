/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include "macrologger.h"

#include "highmap/vector.hpp"

namespace hesiod
{

struct ModelConfig
{
  hmap::Vec2<int> shape = {4, 4};
  hmap::Vec2<int> tiling = {1, 1};
  float           overlap = 0.f;

  void log_debug() const
  {
    LOG_DEBUG("shape: {%d, %d}", shape.x, shape.y);
    LOG_DEBUG("tiling: {%d, %d}", tiling.x, tiling.y);
    LOG_DEBUG("overlap: %f", overlap);
  }
};

} // namespace hesiod