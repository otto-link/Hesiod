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

#include "highmap/coord_frame.hpp"

namespace hesiod
{

// =====================================
// BroadcastParam
// =====================================
struct BroadcastParam
{
  const hmap::CoordFrame *t_source = nullptr;
  const hmap::Heightmap  *p_h = nullptr;
};

using BroadcastMap = std::map<std::string, BroadcastParam>;

// =====================================
// Fcts
// =====================================

bool get_ids_from_broadcast_tag(const std::string &tag,
                                std::string       &graph_id,
                                std::string       &node_id,
                                std::string       &port_id);

} // namespace hesiod
