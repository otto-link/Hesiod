/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

GaborNoise::GaborNoise(std::string     id,
                       hmap::Vec2<int> shape,
                       hmap::Vec2<int> tiling,
                       float           overlap)
    : ControlNode(id), shape(shape), tiling(tiling), overlap(overlap)
{
  LOG_DEBUG("GaborNoise::GaborNoise()");
  this->node_type = "GaborNoise";
  this->category = category_mapping.at(this->node_type);
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void GaborNoise::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

void GaborNoise::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  float density_per_tile = this->density / (float)this->value_out.get_ntiles();
  int   seed0 = this->seed;

  hmap::fill(this->value_out,
             [this, &density_per_tile, &seed0](hmap::Vec2<int> shape)
             {
               return hmap::gabor_noise(shape,
                                        this->kw,
                                        this->angle,
                                        this->width,
                                        density_per_tile,
                                        (uint)seed0++);
             });
  this->value_out.smooth_overlap_buffers();
  this->value_out.remap(this->vmin, this->vmax);
}

} // namespace hesiod::cnode
