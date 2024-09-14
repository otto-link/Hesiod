/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/graph_node.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

GraphNode::GraphNode(const std::string &id, std::shared_ptr<ModelConfig> config)
    : gnode::Graph(), id(id), config(config)
{
  HLOG->trace("GraphNode::GraphNode");
  this->config->log_debug();
}

} // namespace hesiod
