/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Gradient::Gradient(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("Gradient::Gradient()");
  this->node_type = "Gradient";
  this->category = category_mapping.at(this->node_type);
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("dx", gnode::direction::out, dtype::dHeightMap));
  this->add_port(gnode::Port("dy", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void Gradient::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("dx", (void *)&(this->value_out_dx));
  this->set_p_data("dy", (void *)&(this->value_out_dy));
}

void Gradient::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input"));

  this->value_out_dx.set_sto(p_input_hmap->shape,
                             p_input_hmap->tiling,
                             p_input_hmap->overlap);

  this->value_out_dy.set_sto(p_input_hmap->shape,
                             p_input_hmap->tiling,
                             p_input_hmap->overlap);

  hmap::transform(this->value_out_dx, // output
                  *p_input_hmap,      // input
                  [](hmap::Array &out, hmap::Array &in)
                  { hmap::gradient_x(in, out); });

  hmap::transform(this->value_out_dy, // output
                  *p_input_hmap,      // input
                  [](hmap::Array &out, hmap::Array &in)
                  { hmap::gradient_y(in, out); });

  if (this->normalize)
  {
    this->value_out_dx.remap(this->vmin_x, this->vmax_x);
    this->value_out_dy.remap(this->vmin_y, this->vmax_y);
  }
}

} // namespace hesiod::cnode
