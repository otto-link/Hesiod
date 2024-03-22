/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/primitives.hpp"
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Caldera::Caldera(std::string     id,
                 hmap::Vec2<int> shape,
                 hmap::Vec2<int> tiling,
                 float           overlap)
    : ControlNode(id), Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("Caldera::Caldera()");
  this->node_type = "Caldera";
  this->category = category_mapping.at(this->node_type);

  this->attr["radius"] = NEW_ATTR_FLOAT(128.f, 1.f, 512.f);
  this->attr["sigma_inner"] = NEW_ATTR_FLOAT(16.f, 1.f, 512.f);
  this->attr["sigma_outer"] = NEW_ATTR_FLOAT(32.f, 1.f, 512.f);
  this->attr["noise_r_amp"] = NEW_ATTR_FLOAT(32.f, 1.f, 128.f);
  this->attr["z_bottom"] = NEW_ATTR_FLOAT(0.5f, 0.f, 1.f);
  this->attr["noise_ratio_z"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);
  this->attr["center.x"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);
  this->attr["center.y"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);

  this->attr_ordered_key = {"radius",
                            "sigma_inner",
                            "sigma_outer",
                            "noise_r_amp",
                            "z_bottom",
                            "noise_ratio_z",
                            "center.x",
                            "center.y"};
  this->remove_port("dy");
  this->update_inner_bindings();
}

void Caldera::compute()
{
  LOG_DEBUG("computing Caldera node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec4<float> bbox,
                    hmap::Array      *p_noise_x)
             {
               hmap::Vec2<float> center;
               center.x = GET_ATTR_FLOAT("center.x");
               center.y = GET_ATTR_FLOAT("center.y");

               return hmap::caldera(shape,
                                    GET_ATTR_FLOAT("radius"),
                                    GET_ATTR_FLOAT("sigma_inner"),
                                    GET_ATTR_FLOAT("sigma_outer"),
                                    GET_ATTR_FLOAT("z_bottom"),
                                    p_noise_x,
                                    GET_ATTR_FLOAT("noise_r_amp"),
                                    GET_ATTR_FLOAT("noise_ratio_z"),
                                    center,
                                    bbox);
             });

  this->value_out.smooth_overlap_buffers();
  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
