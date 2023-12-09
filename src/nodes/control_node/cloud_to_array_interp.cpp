/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

CloudToArrayInterp::CloudToArrayInterp(std::string     id,
                                       hmap::Vec2<int> shape,
                                       hmap::Vec2<int> tiling,
                                       float           overlap)
    : ControlNode(id), Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("CloudToArrayInterp::CloudToArrayInterp()");
  this->node_type = "CloudToArrayInterp";
  this->category = category_mapping.at(this->node_type);
  this->add_port(gnode::Port("cloud", gnode::direction::in, dtype::dCloud));
  this->update_inner_bindings();
}

void CloudToArrayInterp::compute()
{
  LOG_DEBUG("computing CloudToArrayInterp node [%s]", this->id.c_str());

  hmap::Cloud *p_input_cloud = static_cast<hmap::Cloud *>(
      (void *)this->get_p_data("cloud"));

  if (p_input_cloud->get_npoints() > 0)
    hmap::fill(this->value_out,
               (hmap::HeightMap *)this->get_p_data("dx"),
               (hmap::HeightMap *)this->get_p_data("dy"),
               [this, p_input_cloud](hmap::Vec2<int>   shape,
                                     hmap::Vec2<float> shift,
                                     hmap::Vec2<float> scale,
                                     hmap::Array      *p_noise_x,
                                     hmap::Array      *p_noise_y)
               {
                 hmap::Array array(shape);
                 p_input_cloud->to_array_interp(array,
                                                {0.f, 1.f, 0.f, 1.f},
                                                0, // linear interpolation
                                                p_noise_x,
                                                p_noise_y,
                                                shift,
                                                scale);
                 return array;
               });
  else
    // fill with zeroes
    hmap::transform(this->value_out, [](hmap::Array array) { array = 0.f; });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
