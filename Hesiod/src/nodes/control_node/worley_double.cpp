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
    : ControlNode(id), Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("WorleyDouble::WorleyDouble()");
  this->node_type = "WorleyDouble";
  this->category = category_mapping.at(this->node_type);

  this->attr["kw"] = NEW_ATTR_WAVENB();
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["ratio"] = NEW_ATTR_FLOAT(0.5f, 0.f, 1.f);
  this->attr["k"] = NEW_ATTR_FLOAT(0.05f, 0.f, 1.f);
}

void WorleyDouble::compute()
{
  LOG_DEBUG("computing WorleyDouble node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             (hmap::HeightMap *)this->get_p_data("dy"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec4<float> bbox,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y)
             {
               return hmap::worley_double(shape,
                                          GET_ATTR_WAVENB("kw"),
                                          GET_ATTR_SEED("seed"),
                                          GET_ATTR_FLOAT("ratio"),
                                          GET_ATTR_FLOAT("k"),
                                          p_noise_x,
                                          p_noise_y,
                                          nullptr,
                                          bbox);
             });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
