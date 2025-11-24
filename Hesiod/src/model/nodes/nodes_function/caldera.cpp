/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_caldera_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dr");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("radius", "radius", 0.25f, 0.01f, 1.f);
  node.add_attr<FloatAttribute>("sigma_inner", "sigma_inner", 0.05f, 0.f, 0.3f);
  node.add_attr<FloatAttribute>("sigma_outer", "sigma_outer", 0.1f, 0.f, 0.3f);
  node.add_attr<FloatAttribute>("noise_r_amp", "noise_r_amp", 0.1f, 0.f, 0.3f);
  node.add_attr<FloatAttribute>("z_bottom", "z_bottom", 0.5f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("noise_ratio_z", "noise_ratio_z", 0.1f, 0.f, 1.f);
  node.add_attr<Vec2FloatAttribute>("center", "center");
  node.add_attr<BoolAttribute>("inverse", "inverse", false);
  node.add_attr<RangeAttribute>("remap", "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"radius",
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

void compute_caldera_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_dr = node.get_value_ref<hmap::Heightmap>("dr");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  float radius_pixel = std::max(1.f,
                                node.get_attr<FloatAttribute>("radius") * p_out->shape.x);
  float sigma_inner_pixel = std::max(1.f,
                                     node.get_attr<FloatAttribute>("sigma_inner") *
                                         p_out->shape.x);
  float sigma_outer_pixel = std::max(1.f,
                                     node.get_attr<FloatAttribute>("sigma_outer") *
                                         p_out->shape.x);
  float noise_r_amp_pixel = std::max(1.f,
                                     node.get_attr<FloatAttribute>("noise_r_amp") *
                                         p_out->shape.x);

  hmap::fill(
      *p_out,
      p_dr,
      [&node, &radius_pixel, &sigma_inner_pixel, &sigma_outer_pixel, &noise_r_amp_pixel](
          hmap::Vec2<int>   shape,
          hmap::Vec4<float> bbox,
          hmap::Array      *p_noise)
      {
        return hmap::caldera(shape,
                             radius_pixel,
                             sigma_inner_pixel,
                             sigma_outer_pixel,
                             node.get_attr<FloatAttribute>("z_bottom"),
                             p_noise,
                             noise_r_amp_pixel,
                             node.get_attr<FloatAttribute>("noise_ratio_z"),
                             node.get_attr<Vec2FloatAttribute>("center"),
                             bbox);
      });

  // post-process
  post_process_heightmap(node,
                         *p_out,
                         node.get_attr<BoolAttribute>("inverse"),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         node.get_attr_ref<RangeAttribute>("remap")->get_is_active(),
                         node.get_attr<RangeAttribute>("remap"));
}

} // namespace hesiod
