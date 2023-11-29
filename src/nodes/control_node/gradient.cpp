/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Gradient::Gradient(std::string id) : ControlNode(id)
{
  LOG_DEBUG("Gradient::Gradient()");
  this->node_type = "Gradient";
  this->category = category_mapping.at(this->node_type);

  this->attr["remap.x"] = NEW_ATTR_RANGE();
  this->attr["remap.y"] = NEW_ATTR_RANGE();

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("dx", gnode::direction::out, dtype::dHeightMap));
  this->add_port(gnode::Port("dy", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void Gradient::update_inner_bindings()
{
  this->set_p_data("dx", (void *)&(this->value_out_dx));
  this->set_p_data("dy", (void *)&(this->value_out_dy));
}

void Gradient::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input"));

  // reshaping
  this->value_out_dx.set_sto(p_input_hmap->shape,
                             p_input_hmap->tiling,
                             p_input_hmap->overlap);

  this->value_out_dy.set_sto(p_input_hmap->shape,
                             p_input_hmap->tiling,
                             p_input_hmap->overlap);

  // compute gradients
  hmap::transform(this->value_out_dx, // output
                  *p_input_hmap,      // input
                  [](hmap::Array &out, hmap::Array &in)
                  { hmap::gradient_x(in, out); });

  hmap::transform(this->value_out_dy, // output
                  *p_input_hmap,      // input
                  [](hmap::Array &out, hmap::Array &in)
                  { hmap::gradient_y(in, out); });

  // remap
  if (GET_ATTR_REF_RANGE("remap.x")->is_activated())
  {
    hmap::Vec2<float> vrange = GET_ATTR_RANGE("remap.x");
    this->value_out_dx.remap(vrange.x, vrange.y);
  }

  if (GET_ATTR_REF_RANGE("remap.y")->is_activated())
  {
    hmap::Vec2<float> vrange = GET_ATTR_RANGE("remap.y");
    this->value_out_dy.remap(vrange.x, vrange.y);
  }
}

} // namespace hesiod::cnode
