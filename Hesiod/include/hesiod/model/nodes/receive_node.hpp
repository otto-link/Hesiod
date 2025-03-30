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

class ReceiveNode : public BaseNode
{
public:
  ReceiveNode(const std::string &label, std::shared_ptr<ModelConfig> config)
      : BaseNode(label, config)
  {
  }

  void set_broadcast_parameters(const hmap::Terrain   *new_t_source,
                                const hmap::Heightmap *new_p_h,
                                const hmap::Terrain   *new_t_target,
                                const std::string     &new_tag)
  {
    this->t_source = new_t_source;
    this->p_h = new_p_h;
    this->t_target = new_t_target;
    this->tag = new_tag;
  }

  void update_tag_list(const std::vector<std::string> &new_tags);

  const hmap::Terrain   *t_source = nullptr;
  const hmap::Heightmap *p_h = nullptr;
  const hmap::Terrain   *t_target = nullptr;
  std::string            tag;
};

} // namespace hesiod