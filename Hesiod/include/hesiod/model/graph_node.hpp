/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file graph_node.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "gnode/graph.hpp"

#include "hesiod/model/model_config.hpp"

namespace hesiod
{

class GraphNode : public gnode::Graph
{
public:
  GraphNode() = delete;

  GraphNode(const std::string &id, std::shared_ptr<ModelConfig> config);

  std::string get_id() const { return this->id; }

private:
  std::string                  id;
  std::shared_ptr<ModelConfig> config;
};

} // namespace hesiod