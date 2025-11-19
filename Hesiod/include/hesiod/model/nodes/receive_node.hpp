/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file receive_node.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2025
 */
#pragma once
#include "highmap/coord_frame.hpp"

#include "hesiod/model/graph/broadcast_param.hpp"
#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

struct BroadcastParam; // forward

// =====================================
// ReceiveNode
// =====================================
class ReceiveNode : public BaseNode
{
public:
  ReceiveNode(const std::string &label, std::weak_ptr<GraphConfig> config);

  BroadcastMap     *get_p_broadcast_params();
  std::string       get_current_tag() const;
  hmap::CoordFrame *get_p_coord_frame();
  void              set_p_broadcast_params(BroadcastMap *new_p_broadcast_params);
  void              set_p_coord_frame(hmap::CoordFrame *new_p_coord_frame);

  void update_tag_list(const std::vector<std::string> &new_tags);

private:
  BroadcastMap     *p_broadcast_params = nullptr; // owned by GraphManager
  hmap::CoordFrame *p_coord_frame = nullptr;      // owned by GraphNode
};

} // namespace hesiod
