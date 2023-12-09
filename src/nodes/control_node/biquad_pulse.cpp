/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

BiquadPulse::BiquadPulse(std::string     id,
                         hmap::Vec2<int> shape,
                         hmap::Vec2<int> tiling,
                         float           overlap)
    : ControlNode(id), Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("BiquadPulse::BiquadPulse()");
  this->node_type = "BiquadPulse";
  this->category = category_mapping.at(this->node_type);

  this->attr["gain"] = NEW_ATTR_FLOAT(1.f, 0.01f, 10.f);
}

void BiquadPulse::compute()
{
  LOG_DEBUG("computing BiquadPulse node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             (hmap::HeightMap *)this->get_p_data("dy"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec2<float> shift,
                    hmap::Vec2<float> scale,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y)
             {
               return hmap::biquad_pulse(shape,
                                         GET_ATTR_FLOAT("gain"),
                                         p_noise_x,
                                         p_noise_y,
                                         shift,
                                         scale);
             });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
