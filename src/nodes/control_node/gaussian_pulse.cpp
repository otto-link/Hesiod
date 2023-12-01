/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

GaussianPulse::GaussianPulse(std::string     id,
                             hmap::Vec2<int> shape,
                             hmap::Vec2<int> tiling,
                             float           overlap)
    : Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("GaussianPulse::GaussianPulse()");
  this->node_type = "GaussianPulse";
  this->category = category_mapping.at(this->node_type);

  this->attr["sigma"] = NEW_ATTR_FLOAT(32.f, 0.01f, 256.f);
  this->attr["center.x"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);
  this->attr["center.y"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);

  this->attr_ordered_key = {"sigma", "center.x", "center.y"};

  this->remove_port("dy");
  this->update_inner_bindings();
}

void GaussianPulse::compute()
{
  LOG_DEBUG("computing GaussianPulse node [%s]", this->id.c_str());

  hmap::Vec2<float> center;
  center.x = GET_ATTR_FLOAT("center.x");
  center.y = GET_ATTR_FLOAT("center.y");

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             [this, &center](hmap::Vec2<int>   shape,
                             hmap::Vec2<float> shift,
                             hmap::Vec2<float> scale,
                             hmap::Array      *p_noise_x)
             {
               return hmap::gaussian_pulse(shape,
                                           GET_ATTR_FLOAT("sigma"),
                                           p_noise_x,
                                           center,
                                           shift,
                                           scale);
             });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
