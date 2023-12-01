/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Filter::Filter(std::string id) : ControlNode(id)
{
  LOG_DEBUG("Filter::Filter()");
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("mask",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void Filter::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void Filter::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_input_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  hmap::HeightMap *p_input_mask = CAST_PORT_REF(hmap::HeightMap, "mask");

  // work on a copy of the input
  this->value_out = *p_input_hmap;
  this->compute_filter(this->value_out, p_input_mask);
}

} // namespace hesiod::cnode
