/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file broadcast_node.hpp
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

class BroadcastNode : public BaseNode
{
public:
  BroadcastNode(const std::string &label, std::shared_ptr<ModelConfig> config)
      : BaseNode(label, config)
  {
  }

  void generate_broadcast_tag();

  std::string get_broadcast_tag() const { return this->broadcast_tag; }

  void json_from(nlohmann::json const &json) override;

  nlohmann::json json_to() const override;

private:
  std::string broadcast_tag = "UNDEFINED";
};

} // namespace hesiod