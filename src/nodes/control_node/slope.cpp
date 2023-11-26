/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Slope::Slope(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap)
    : ControlNode(id), shape(shape), tiling(tiling), overlap(overlap)
{
  LOG_DEBUG("Slope::Slope()");
  this->node_type = "Slope";
  this->category = category_mapping.at(this->node_type);

  this->add_port(gnode::Port("dx",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));

  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void Slope::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

void Slope::compute()
{
  LOG_DEBUG("computing Slope node [%s]", this->id.c_str());

  float talus = this->talus_global / (float)this->value_out.shape.x;

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             [this, &talus](hmap::Vec2<int>   shape,
                            hmap::Vec2<float> shift,
                            hmap::Vec2<float> scale,
                            hmap::Array      *p_noise_x)
             {
               return hmap::slope(shape,
                                  this->angle,
                                  talus,
                                  p_noise_x,
                                  this->center,
                                  shift,
                                  scale);
             });
}

} // namespace hesiod::cnode
