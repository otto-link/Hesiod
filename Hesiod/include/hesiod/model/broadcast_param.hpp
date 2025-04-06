/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file graph_manager.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2025
 */
#pragma once
#include <map>
#include <string>

#include "highmap/terrain/terrain.hpp"

namespace hesiod
{

// =====================================
// BroadcastParam
// =====================================
struct BroadcastParam
{
  const hmap::Terrain   *t_source = nullptr;
  const hmap::Heightmap *p_h = nullptr;
};

using BroadcastMap = std::map<std::string, BroadcastParam>;

} // namespace hesiod