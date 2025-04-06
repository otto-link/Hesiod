/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file graph_node.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2023
 */
#pragma once
#include "nlohmann/json.hpp"

#include "gnode/graph.hpp"
#include "highmap/terrain/terrain.hpp"

#include "hesiod/model/model_config.hpp"

namespace hesiod
{

// =====================================
// GraphNode
// =====================================
class GraphNode : public gnode::Graph, public hmap::Terrain
{
public:
  GraphNode() = delete;
  GraphNode(const std::string &id, std::shared_ptr<ModelConfig> config);

  // --- Serialization ---
  void           json_from(nlohmann::json const &json, bool override_config = true);
  nlohmann::json json_to() const;

  // --- Config. ---
  ModelConfig *get_config_ref() { return this->config.get(); }

  // --- Node Factory (create nodes from their type) ---
  std::string new_node(const std::string &node_type);

private:
  // --- Members ---
  std::shared_ptr<ModelConfig> config;
};

} // namespace hesiod