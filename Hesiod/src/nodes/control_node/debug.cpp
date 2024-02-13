/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Debug::Debug(std::string id) : ControlNode(id)
{
  LOG_DEBUG("Debug::Debug()");
  this->node_type = "Debug";
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->category = category_mapping.at(this->node_type);
}

void Debug::compute()
{
}

} // namespace hesiod::cnode
