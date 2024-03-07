/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Warp::Warp(std::string id) : ControlNode(id)
{
  this->node_type = "Warp";
  this->category = category_mapping.at(this->node_type);

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("dx",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(gnode::Port("dy",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void Warp::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void Warp::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  // work on a copy of the input
  this->value_out = *CAST_PORT_REF(hmap::HeightMap, "input");

  hmap::transform(this->value_out,
                  CAST_PORT_REF(hmap::HeightMap, "dx"),
                  CAST_PORT_REF(hmap::HeightMap, "dy"),
                  [this](hmap::Array &z, hmap::Array *p_dx, hmap::Array *p_dy)
                  { hmap::warp(z, p_dx, p_dy); });

  this->value_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
