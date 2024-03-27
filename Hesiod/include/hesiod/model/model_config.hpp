/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include "macrologger.h"

#include "highmap/vector.hpp"

#define HSD_PREVIEW_SHAPE 128

namespace hesiod
{

struct ModelConfig
{
  // --- model
  hmap::Vec2<int> shape = {4, 4};
  hmap::Vec2<int> tiling = {1, 1};
  float           overlap = 0.f;

  void log_debug() const
  {
    LOG_DEBUG("shape: {%d, %d}", shape.x, shape.y);
    LOG_DEBUG("tiling: {%d, %d}", tiling.x, tiling.y);
    LOG_DEBUG("overlap: %f", overlap);
    LOG_DEBUG("shape_preview: {%d, %d}", shape_preview.x, shape_preview.y);
  }

  void set_shape(hmap::Vec2<int> new_shape)
  {
    this->shape = new_shape;
    this->shape_preview = {std::min(HSD_PREVIEW_SHAPE, this->shape.x),
                           std::min(HSD_PREVIEW_SHAPE, this->shape.y)};
  }

  // --- GUI
  hmap::Vec2<int> shape_preview = {std::min(HSD_PREVIEW_SHAPE, shape.x),
                                   std::min(HSD_PREVIEW_SHAPE, shape.y)};
};

} // namespace hesiod