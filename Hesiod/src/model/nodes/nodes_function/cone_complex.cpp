/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_cone_complex_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "radius", 0.5f, 0.01f, FLT_MAX);
  ADD_ATTR(node, FloatAttribute, "alpha", 1.2f, 0.01f, 4.f);
  ADD_ATTR(node, BoolAttribute, "smooth_profile", true);
  ADD_ATTR(node, FloatAttribute, "valley_amp", 0.2f, 0.f, 2.f);
  ADD_ATTR(node, IntAttribute, "valley_nb", 5, 0, 32);
  ADD_ATTR(node, FloatAttribute, "valley_decay_ratio", 0.5f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "valley_angle0", 0.f, -180.f, 180.f, "{:.1f}°");
  ADD_ATTR(node,
           EnumAttribute,
           "erosion_profile",
           enum_mappings.erosion_profile_map,
           "Triangle Grenier");
  ADD_ATTR(node, FloatAttribute, "erosion_delta", 0.01f, 0.f, 0.2f);
  ADD_ATTR(node, FloatAttribute, "radial_waviness_amp", 0.05f, 0.f, 0.5f);
  ADD_ATTR(node, FloatAttribute, "radial_waviness_kw", 2.f, 0.f, 16.f);
  ADD_ATTR(node, FloatAttribute, "bias_angle", 30.f, -180.f, 180.f, "{:.1f}°");
  ADD_ATTR(node, FloatAttribute, "bias_amp", 0.1f, 0.f, 2.f);
  ADD_ATTR(node, FloatAttribute, "bias_exponent", 1.f, 0.01f, 4.f);
  ADD_ATTR(node, Vec2FloatAttribute, "center");

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Main Parameters",
                             //
                             "_TEXT_Shape & Size",
                             "radius",
                             "alpha",
                             "smooth_profile",
                             //
                             "_TEXT_Position",
                             "center",
                             //
                             "_TEXT_Erosion valleys",
                             "valley_amp",
                             "valley_nb",
                             "valley_decay_ratio",
                             "valley_angle0",
                             "erosion_profile",
                             "erosion_delta",
                             "radial_waviness_amp",
                             "radial_waviness_kw",
                             //
                             "_TEXT_Directional bias",
                             "bias_angle",
                             "bias_amp",
                             "bias_exponent",
                             //
                             "_GROUPBOX_END_"});

  setup_post_process_heightmap_attributes(node);
}

void compute_cone_complex_node(BaseNode &node)
{

  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_ctrl = node.get_value_ref<hmap::Heightmap>("control");
  hmap::Heightmap *p_env = node.get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  hmap::transform(
      {p_out, p_ctrl, p_dx, p_dy},
      [&node](std::vector<hmap::Array *> p_arrays,
              hmap::Vec2<int>            shape,
              hmap::Vec4<float>          bbox)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::Array *pa_ctrl = p_arrays[1];
        hmap::Array *pa_dx = p_arrays[2];
        hmap::Array *pa_dy = p_arrays[3];

        *pa_out = hmap::cone_complex(
            shape,
            GET(node, "alpha", FloatAttribute),
            GET(node, "radius", FloatAttribute),
            GET(node, "smooth_profile", BoolAttribute),
            GET(node, "valley_amp", FloatAttribute),
            GET(node, "valley_nb", IntAttribute),
            GET(node, "valley_decay_ratio", FloatAttribute),
            GET(node, "valley_angle0", FloatAttribute),
            (hmap::ErosionProfile)GET(node, "erosion_profile", EnumAttribute),
            GET(node, "erosion_delta", FloatAttribute),
            GET(node, "radial_waviness_amp", FloatAttribute),
            GET(node, "radial_waviness_kw", FloatAttribute),
            GET(node, "bias_angle", FloatAttribute),
            GET(node, "bias_amp", FloatAttribute),
            GET(node, "bias_exponent", FloatAttribute),
            GET(node, "center", Vec2FloatAttribute),
            pa_ctrl,
            pa_dx,
            pa_dy,
            bbox);
      },
      node.get_config_ref()->hmap_transform_mode_cpu);

  // post-process
  post_apply_enveloppe(node, *p_out, p_env);
  post_process_heightmap(node, *p_out);

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
