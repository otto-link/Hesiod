/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

PerlinBillow::PerlinBillow(std::string     id,
                           hmap::Vec2<int> shape,
                           hmap::Vec2<int> tiling,
                           float           overlap)
    : Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("PerlinBillow::PerlinBillow()");
  this->node_type = "PerlinBillow";
  this->category = category_mapping.at(this->node_type);

  this->attr["kw"] = NEW_ATTR_WAVENB();
  this->attr["seed"] = NEW_ATTR_SEED();
}

void PerlinBillow::compute()
{
  LOG_DEBUG("computing PerlinBillow node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             (hmap::HeightMap *)this->get_p_data("dy"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec2<float> shift,
                    hmap::Vec2<float> scale,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y)
             {
               return hmap::perlin_billow(shape,
                                          GET_ATTR_WAVENB("kw"),
                                          GET_ATTR_SEED("seed"),
                                          p_noise_x,
                                          p_noise_y,
                                          shift,
                                          scale);
             });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
