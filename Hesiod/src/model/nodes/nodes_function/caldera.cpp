/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_caldera_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dr");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "radius", 0.25f, 0.01f, 1.f);
  ADD_ATTR(FloatAttribute, "sigma_inner", 0.05f, 0.f, 0.3f);
  ADD_ATTR(FloatAttribute, "sigma_outer", 0.1f, 0.f, 0.3f);
  ADD_ATTR(FloatAttribute, "noise_r_amp", 0.1f, 0.f, 0.3f);
  ADD_ATTR(FloatAttribute, "z_bottom", 0.5f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "noise_ratio_z", 0.1f, 0.f, 1.f);
  ADD_ATTR(Vec2FloatAttribute, "center");
  ADD_ATTR(BoolAttribute, "inverse", false);
  ADD_ATTR(RangeAttribute, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key({"radius",
                                "sigma_inner",
                                "sigma_outer",
                                "noise_r_amp",
                                "z_bottom",
                                "noise_ratio_z",
                                "center",
                                "_SEPARATOR_",
                                "inverse",
                                "remap"});
}

void compute_caldera_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  // base noise function
  hmap::Heightmap *p_dr = p_node->get_value_ref<hmap::Heightmap>("dr");
  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

  float radius_pixel = std::max(1.f, GET("radius", FloatAttribute) * p_out->shape.x);
  float sigma_inner_pixel = std::max(1.f,
                                     GET("sigma_inner", FloatAttribute) * p_out->shape.x);
  float sigma_outer_pixel = std::max(1.f,
                                     GET("sigma_outer", FloatAttribute) * p_out->shape.x);
  float noise_r_amp_pixel = std::max(1.f,
                                     GET("noise_r_amp", FloatAttribute) * p_out->shape.x);

  hmap::fill(
      *p_out,
      p_dr,
      [p_node, &radius_pixel, &sigma_inner_pixel, &sigma_outer_pixel, &noise_r_amp_pixel](
          hmap::Vec2<int>   shape,
          hmap::Vec4<float> bbox,
          hmap::Array      *p_noise)
      {
        return hmap::caldera(shape,
                             radius_pixel,
                             sigma_inner_pixel,
                             sigma_outer_pixel,
                             GET("z_bottom", FloatAttribute),
                             p_noise,
                             noise_r_amp_pixel,
                             GET("noise_ratio_z", FloatAttribute),
                             GET("center", Vec2FloatAttribute),
                             bbox);
      });

  // post-process
  post_process_heightmap(p_node,
                         *p_out,
                         GET("inverse", BoolAttribute),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_ATTR("remap", RangeAttribute, is_active),
                         GET("remap", RangeAttribute));

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
