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
    : ControlNode(id), Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("GaussianPulse::GaussianPulse()");
  this->node_type = "GaussianPulse";
  this->category = category_mapping.at(this->node_type);

  this->attr["radius"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);
  this->attr["center.x"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);
  this->attr["center.y"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);

  this->attr_ordered_key = {"radius", "center.x", "center.y"};

  this->remove_port("dy");
  this->update_inner_bindings();
}

void GaussianPulse::compute()
{
  LOG_DEBUG("computing GaussianPulse node [%s]", this->id.c_str());

  hmap::Vec2<float> center;
  center.x = GET_ATTR_FLOAT("center.x");
  center.y = GET_ATTR_FLOAT("center.y");

  float sigma = std::max(1.f,
                         (GET_ATTR_FLOAT("radius") * this->value_out.shape.x));

  hmap::fill(
      this->value_out,
      (hmap::HeightMap *)this->get_p_data("dx"),
      [this, &center, &sigma](hmap::Vec2<int>   shape,
                              hmap::Vec4<float> bbox,
                              hmap::Array      *p_noise_x)
      { return hmap::gaussian_pulse(shape, sigma, p_noise_x, center, bbox); });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
