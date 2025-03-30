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
#include "highmap/terrain/terrain.hpp"

#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

struct BroadcastParam; // forward

class ReceiveNode : public BaseNode
{
public:
  ReceiveNode(const std::string &label, std::shared_ptr<ModelConfig> config)
      : BaseNode(label, config)
  {
  }

  std::map<std::string, BroadcastParam> *get_p_broadcast_params()
  {
    return this->p_broadcast_params;
  }

  hmap::Terrain *get_p_target_terrain() { return this->p_target_terrain; }

  void set_p_broadcast_params(
      std::map<std::string, BroadcastParam> *new_p_broadcast_params)
  {
    this->p_broadcast_params = new_p_broadcast_params;
  }

  void set_p_target_terrain(hmap::Terrain *new_p_target_terrain)
  {
    this->p_target_terrain = new_p_target_terrain;
  }

  void update_tag_list(const std::vector<std::string> &new_tags);

private:
  // ownership by GraphManager
  std::map<std::string, BroadcastParam> *p_broadcast_params = nullptr;

  hmap::Terrain *p_target_terrain = nullptr;
};

} // namespace hesiod