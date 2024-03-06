/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ValueNoiseDelaunay::ValueNoiseDelaunay(std::string     id,
                                       hmap::Vec2<int> shape,
                                       hmap::Vec2<int> tiling,
                                       float           overlap)
    : ControlNode(id), Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("ValueNoiseDelaunay::ValueNoiseDelaunay()");
  this->node_type = "ValueNoiseDelaunay";
  this->category = category_mapping.at(this->node_type);

  this->attr["kw"] = NEW_ATTR_WAVENB();
  this->attr["seed"] = NEW_ATTR_SEED();
}

void ValueNoiseDelaunay::compute()
{
  LOG_DEBUG("computing ValueNoiseDelaunay node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             (hmap::HeightMap *)this->get_p_data("dy"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec4<float> bbox,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y)
             {
               return hmap::value_noise_delaunay(shape,
                                                 GET_ATTR_WAVENB("kw"),
                                                 GET_ATTR_SEED("seed"),
                                                 p_noise_x,
                                                 p_noise_y,
                                                 nullptr,
                                                 bbox);
             });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
