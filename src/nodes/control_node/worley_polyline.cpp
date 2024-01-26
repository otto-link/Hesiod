/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

WorleyPolyline::WorleyPolyline(std::string     id,
                               hmap::Vec2<int> shape,
                               hmap::Vec2<int> tiling,
                               float           overlap)
    : ControlNode(id), Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("WorleyPolyline::WorleyPolyline()");
  this->node_type = "WorleyPolyline";
  this->category = category_mapping.at(this->node_type);

  this->attr["kw"] = NEW_ATTR_FLOAT(4.f, 0.01f, 64.f);
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["decay"] = NEW_ATTR_FLOAT(1.f, 0.01f, 4.f);

  this->attr_ordered_key = {"kw", "seed", "decay"};
}

void WorleyPolyline::compute()
{
  LOG_DEBUG("computing WorleyPolyline node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             (hmap::HeightMap *)this->get_p_data("dy"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec2<float> shift,
                    hmap::Vec2<float> scale,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y)
             {
               return hmap::worley_polyline(shape,
                                            GET_ATTR_FLOAT("kw"),
                                            GET_ATTR_SEED("seed"),
                                            GET_ATTR_FLOAT("decay"),
                                            p_noise_x,
                                            p_noise_y,
                                            shift,
                                            scale);
             });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
