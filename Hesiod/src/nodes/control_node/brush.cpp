/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Brush::Brush(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap)
    : ControlNode(id), shape(shape), tiling(tiling), overlap(overlap)
{
  LOG_DEBUG("Brush::Brush()");
  this->node_type = "Brush";
  this->category = category_mapping.at(this->node_type);
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void Brush::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

void Brush::compute()
{
  LOG_DEBUG("computing Perlin node [%s]", this->id.c_str());

  // remap the output
  if (this->remap)
  {
    if (this->inverse)
      this->value_out.remap(this->vmax, this->vmin);
    else
      this->value_out.remap(this->vmin, this->vmax);
  }
}

} // namespace hesiod::cnode
