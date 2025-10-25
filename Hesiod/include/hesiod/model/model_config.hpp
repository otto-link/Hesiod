/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once

#include "nlohmann/json.hpp"

#include "highmap/algebra.hpp"
#include "highmap/heightmap.hpp"

namespace hesiod
{

struct ModelConfig
{
  ModelConfig();

  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;

  void log_debug() const;
  void set_shape(const hmap::Vec2<int> &new_shape) { this->shape = new_shape; }

  // --- Members
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;

  hmap::TransformMode hmap_transform_mode_cpu;
  hmap::TransformMode hmap_transform_mode_gpu;
};

} // namespace hesiod
