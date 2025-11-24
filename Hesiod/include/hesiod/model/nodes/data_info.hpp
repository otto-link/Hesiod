/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <string>

#include "highmap/array.hpp"
#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/heightmap.hpp"

#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

template <typename T> std::string get_data_info(BaseNode *node, const std::string &key)
{
  auto *p_val = node->get_value_ref<T>(key);
  if (!p_val)
    return "-";

  if constexpr (std::is_same_v<T, hmap::Heightmap>)
  {
    return std::format("[{:.3e}, {:.3e}]", p_val->min(), p_val->max());
  }
  else if constexpr (std::is_same_v<T, hmap::Array>)
  {
    return std::format("[{:.3e}, {:.3e}]", p_val->min(), p_val->max());
  }
  else if constexpr (std::is_same_v<T, hmap::Cloud> || std::is_same_v<T, hmap::Path>)
  {
    return std::format("[{:.3e}, {:.3e}], count: {}",
                       p_val->get_values_min(),
                       p_val->get_values_max(),
                       p_val->get_npoints());
  }
  else
  {
    return "-";
  }
}

} // namespace hesiod