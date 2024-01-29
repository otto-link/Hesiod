/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Unary::Unary(std::string id) : ControlNode(id)
{
  LOG_DEBUG("Unary::Unary()");
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void Unary::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void Unary::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_input_hmap = CAST_PORT_REF(hmap::HeightMap, "input");

  this->value_out.set_sto(p_input_hmap->shape,
                          p_input_hmap->tiling,
                          p_input_hmap->overlap);

  this->compute_in_out(this->value_out, p_input_hmap);
  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
