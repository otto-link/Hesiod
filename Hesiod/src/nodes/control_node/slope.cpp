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
    : ControlNode(id), Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("Slope::Slope()");
  this->node_type = "Slope";
  this->category = category_mapping.at(this->node_type);

  this->attr["angle"] = NEW_ATTR_FLOAT(0.f, -180.f, 180.f);
  this->attr["talus_global"] = NEW_ATTR_FLOAT(4.f, 0.f, 8.f);
  this->attr["center.x"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);
  this->attr["center.y"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);

  this->attr_ordered_key = {"angle", "talus_global", "center.x", "center.y"};

  this->remove_port("dy");
  this->update_inner_bindings();
}

void Slope::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

void Slope::compute()
{
  LOG_DEBUG("computing Slope node [%s]", this->id.c_str());

  hmap::Vec2<float> center;
  center.x = GET_ATTR_FLOAT("center.x");
  center.y = GET_ATTR_FLOAT("center.y");

  float talus = GET_ATTR_FLOAT("talus_global") / (float)this->value_out.shape.x;

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             [this, &talus, &center](hmap::Vec2<int>   shape,
                                     hmap::Vec4<float> bbox,
                                     hmap::Array      *p_noise_x)
             {
               return hmap::slope(shape,
                                  GET_ATTR_FLOAT("angle"),
                                  talus,
                                  p_noise_x,
                                  nullptr,
                                  nullptr,
                                  center,
                                  bbox);
             });
}

} // namespace hesiod::cnode
