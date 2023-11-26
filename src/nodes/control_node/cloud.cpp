/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Cloud::Cloud(std::string id) : ControlNode(id)
{
  LOG_DEBUG("Cloud::Cloud()");
  this->node_type = "Cloud";
  this->category = category_mapping.at(this->node_type);
  this->add_port(gnode::Port("output", gnode::direction::out, dtype::dCloud));
  this->update_inner_bindings();
}

void Cloud::compute()
{
  LOG_DEBUG("computing Cloud node [%s]", this->id.c_str());
  // nothing here
}

void Cloud::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
