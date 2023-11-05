/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Preview::Preview(std::string id) : gnode::Node(id)
{
  this->node_type = "Preview";
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("colorize",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->category = category_mapping.at(this->node_type);
  this->update_inner_bindings();
}

void Preview::update_inner_bindings()
{
  this->set_p_data("output", this->get_p_data("input"));
}

void Preview::compute()
{
  // nothing here
}

} // namespace hesiod::cnode
