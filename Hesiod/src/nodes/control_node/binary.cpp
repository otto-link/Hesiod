/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Binary::Binary(std::string id) : ControlNode(id)
{
  LOG_DEBUG("Binary::Binary()");
  this->add_port(
      gnode::Port("input##1", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("input##2", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void Binary::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void Binary::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_hmap1 = CAST_PORT_REF(hmap::HeightMap, "input##1");
  hmap::HeightMap *p_hmap2 = CAST_PORT_REF(hmap::HeightMap, "input##2");

  this->value_out.set_sto(p_hmap1->shape, p_hmap1->tiling, p_hmap1->overlap);

  this->compute_in_out(this->value_out, p_hmap1, p_hmap2);
  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
