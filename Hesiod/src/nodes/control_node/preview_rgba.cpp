/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

PreviewRGBA::PreviewRGBA(std::string id) : ControlNode(id)
{
  this->node_type = "PreviewRGBA";
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("RGBA",
                             gnode::direction::in,
                             dtype::dHeightMapRGBA,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("thru input", gnode::direction::out, dtype::dHeightMap));
  this->add_port(
      gnode::Port("thru RGBA", gnode::direction::out, dtype::dHeightMapRGBA));
  this->category = category_mapping.at(this->node_type);
  this->update_inner_bindings();
}

void PreviewRGBA::update_inner_bindings()
{
  this->set_p_data("thru input", this->get_p_data("input"));
  this->set_p_data("thru RGBA", this->get_p_data("RGBA"));
}

void PreviewRGBA::compute()
{
  // for thru port
  this->update_inner_bindings();
  this->update_links();
}

} // namespace hesiod::cnode
