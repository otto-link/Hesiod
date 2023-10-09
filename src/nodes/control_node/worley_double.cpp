/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

WorleyDouble::WorleyDouble(std::string     id,
                           hmap::Vec2<int> shape,
                           hmap::Vec2<int> tiling,
                           float           overlap)
    : Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("WorleyDouble::WorleyDouble()");
  this->node_type = "WorleyDouble";
  this->category = category_mapping.at(this->node_type);
  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void WorleyDouble::compute()
{
  LOG_DEBUG("computing WorleyDouble node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             (hmap::HeightMap *)this->get_p_data("dy"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec2<float> shift,
                    hmap::Vec2<float> scale,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y)
             {
               return hmap::worley_double(shape,
                                          this->kw,
                                          (uint)this->seed,
                                          this->ratio,
                                          this->k,
                                          p_noise_x,
                                          p_noise_y,
                                          shift,
                                          scale);
             });

  // remap the output
  this->value_out.remap(this->vmin, this->vmax);
}

} // namespace hesiod::cnode
