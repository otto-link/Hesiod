/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Checkerboard::Checkerboard(std::string     id,
                           hmap::Vec2<int> shape,
                           hmap::Vec2<int> tiling,
                           float           overlap)
    : Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("Checkerboard::Checkerboard()");
  this->node_type = "Checkerboard";
  this->category = category_mapping.at(this->node_type);
  this->value_out.set_shape(shape);
  this->update_inner_bindings();
}

void Checkerboard::compute()
{
  LOG_DEBUG("computing Checkerboard node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             (hmap::HeightMap *)this->get_p_data("dy"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec2<float> shift,
                    hmap::Vec2<float> scale,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y)
             {
               return hmap::checkerboard(shape,
                                         this->kw,
                                         p_noise_x,
                                         p_noise_y,
                                         shift,
                                         scale);
             });

  // remap the output
  this->value_out.remap(this->vmin, this->vmax);
}

} // namespace hesiod::cnode
