/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Perlin::Perlin(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap)
    : Primitive(id, shape, tiling, overlap)
{
  this->node_type = "Perlin";
  this->category = category_mapping.at(this->node_type);
  this->value_out.set_shape(shape);
  this->update_inner_bindings();
}

hmap::Vec2<float> Perlin::get_kw()
{
  return this->kw;
}

uint Perlin::get_seed()
{
  return this->seed;
}

void Perlin::set_kw(hmap::Vec2<float> new_kw)
{
  if (new_kw != this->kw)
  {
    this->kw = new_kw;
    this->force_update();
  }
}

void Perlin::set_seed(uint new_seed)
{
  if (new_seed != this->seed)
  {
    this->seed = new_seed;
    this->force_update();
  }
}

void Perlin::compute()
{
  LOG_DEBUG("computing Perlin node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             (hmap::HeightMap *)this->get_p_data("dy"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec2<float> shift,
                    hmap::Vec2<float> scale,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y)
             {
               return hmap::perlin(shape,
                                   this->kw,
                                   this->seed,
                                   p_noise_x,
                                   p_noise_y,
                                   shift,
                                   scale);
             });

  // remap the output
  this->value_out.remap(this->get_vmin(), this->get_vmax());
}

} // namespace hesiod::cnode
