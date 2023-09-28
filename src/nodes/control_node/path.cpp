/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Path::Path(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("Path::Path()");
  this->node_type = "Path";
  this->category = category_mapping.at(this->node_type);
  this->add_port(gnode::Port("output", gnode::direction::out, dtype::dPath));
  this->update_inner_bindings();
}

void Path::compute()
{
  this->value_out.closed = this->closed;
}

void Path::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
