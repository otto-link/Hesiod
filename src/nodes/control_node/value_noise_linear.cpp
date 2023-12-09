/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ValueNoiseLinear::ValueNoiseLinear(std::string     id,
                                   hmap::Vec2<int> shape,
                                   hmap::Vec2<int> tiling,
                                   float           overlap)
    : ControlNode(id), Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("ValueNoiseLinear::ValueNoiseLinear()");
  this->node_type = "ValueNoiseLinear";
  this->category = category_mapping.at(this->node_type);

  this->attr["kw"] = NEW_ATTR_WAVENB();
  this->attr["seed"] = NEW_ATTR_SEED();
}

void ValueNoiseLinear::compute()
{
  LOG_DEBUG("computing ValueNoiseLinear node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             (hmap::HeightMap *)this->get_p_data("dy"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec2<float> shift,
                    hmap::Vec2<float> scale,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y)
             {
               return hmap::value_noise_linear(shape,
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
