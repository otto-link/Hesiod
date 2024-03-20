/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

PreviewColorizeRGBA::PreviewColorizeRGBA(std::string id) : ControlNode(id)
{
  this->node_type = "PreviewColorizeRGBA";
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("RGBA",
                             gnode::direction::in,
                             dtype::dHeightMapRGBA,
                             gnode::optional::yes));
  this->add_port(gnode::Port("thru", gnode::direction::out, dtype::dHeightMap));
  this->category = category_mapping.at(this->node_type);
  this->update_inner_bindings();
}

void PreviewColorizeRGBA::update_inner_bindings()
{
  this->set_p_data("thru", this->get_p_data("input"));
}

void PreviewColorizeRGBA::compute()
{
  // for thru port
  this->update_inner_bindings();
  this->update_links();
}

} // namespace hesiod::cnode
