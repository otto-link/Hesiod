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
  hmap::HeightMap *p_input_hmap1 = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input##1"));
  hmap::HeightMap *p_input_hmap2 = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input##2"));

  this->value_out.set_sto(p_input_hmap1->shape,
                          p_input_hmap1->tiling,
                          p_input_hmap1->overlap);

  this->compute_in_out(this->value_out, p_input_hmap1, p_input_hmap2);
}

} // namespace hesiod::cnode
