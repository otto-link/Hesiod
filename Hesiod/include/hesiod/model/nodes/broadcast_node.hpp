/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

// =====================================
// BroadcastNode
// =====================================
class BroadcastNode : public BaseNode
{
public:
  BroadcastNode(const std::string &label, std::weak_ptr<GraphConfig> config);

  void        generate_broadcast_tag();
  std::string get_broadcast_tag();

  void           json_from(nlohmann::json const &json) override;
  nlohmann::json json_to() const override;

  // --- Callback
  std::function<void(const std::string &graph_id, const std::string &tag)>
      broadcast_node_updated;

private:
  std::string broadcast_tag = "UNDEFINED";
};

} // namespace hesiod
