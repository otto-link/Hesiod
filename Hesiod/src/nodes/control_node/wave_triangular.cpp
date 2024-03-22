/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/primitives.hpp"
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

WaveTriangular::WaveTriangular(std::string     id,
                               hmap::Vec2<int> shape,
                               hmap::Vec2<int> tiling,
                               float           overlap)
    : ControlNode(id), Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("WaveTriangular::WaveTriangular()");
  this->node_type = "WaveTriangular";
  this->category = category_mapping.at(this->node_type);

  this->attr["kw"] = NEW_ATTR_FLOAT(2.f, 0.01f, 128.f);
  this->attr["angle"] = NEW_ATTR_FLOAT(0.f, -180.f, 180.f);
  this->attr["slant_ratio"] = NEW_ATTR_FLOAT(0.2f, 0.f, 1.f);
  this->attr["phase_shift"] = NEW_ATTR_FLOAT(0.f, -180.f, 180.f);

  this->attr_ordered_key = {"kw", "angle", "slant_ratio", "phase_shift"};

  this->remove_port("dy");
  this->update_inner_bindings();
}

void WaveTriangular::compute()
{
  LOG_DEBUG("computing WaveTriangular node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec4<float> bbox,
                    hmap::Array      *p_noise_x)
             {
               return hmap::wave_triangular(shape,
                                            GET_ATTR_FLOAT("kw"),
                                            GET_ATTR_FLOAT("angle"),
                                            GET_ATTR_FLOAT("slant_ratio"),
                                            GET_ATTR_FLOAT("phase_shift"),
                                            p_noise_x,
                                            nullptr,
                                            nullptr,
                                            bbox);
             });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
