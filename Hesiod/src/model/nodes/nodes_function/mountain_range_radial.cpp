/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_mountain_range_radial_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "angle", CONFIG);

  // attribute(s)
  ADD_ATTR(WaveNbAttribute, "kw", std::vector<float>(2, 4.f), 0.f, FLT_MAX, true);
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(FloatAttribute, "half_width", 0.2f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "angle_spread_ratio", 0.5f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "core_size_ratio", 0.2f, 0.01f, 2.f);
  ADD_ATTR(Vec2FloatAttribute, "center");
  ADD_ATTR(IntAttribute, "octaves", 8, 0, 32);
  ADD_ATTR(FloatAttribute, "weight", 0.7f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "persistence", 0.5f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "lacunarity", 2.f, 0.01f, 4.f);
  ADD_ATTR(BoolAttribute, "inverse", false);
  ADD_ATTR(RangeAttribute, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key({"kw",
                                "seed",
                                "half_width",
                                "angle_spread_ratio",
                                "core_size_ratio",
                                "center",
                                "_SEPARATOR_",
                                "octaves",
                                "weight",
                                "persistence",
                                "lacunarity",
                                "_SEPARATOR_",
                                "inverse",
                                "remap"});
}

void compute_mountain_range_radial_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // base noise function
  hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_ctrl = p_node->get_value_ref<hmap::Heightmap>("control");
  hmap::Heightmap *p_env = p_node->get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
  hmap::Heightmap *p_angle = p_node->get_value_ref<hmap::Heightmap>("angle");

  hmap::transform(
      {p_out, p_ctrl, p_dx, p_dy, p_angle},
      [p_node](std::vector<hmap::Array *> p_arrays,
               hmap::Vec2<int>            shape,
               hmap::Vec4<float>          bbox)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::Array *pa_ctrl = p_arrays[1];
        hmap::Array *pa_dx = p_arrays[2];
        hmap::Array *pa_dy = p_arrays[3];
        hmap::Array *pa_angle = p_arrays[4];

        *pa_out = hmap::gpu::mountain_range_radial(
            shape,
            GET("kw", WaveNbAttribute),
            GET("seed", SeedAttribute),
            GET("half_width", FloatAttribute),
            GET("angle_spread_ratio", FloatAttribute),
            GET("core_size_ratio", FloatAttribute),
            GET("center", Vec2FloatAttribute),
            GET("octaves", IntAttribute),
            GET("weight", FloatAttribute),
            GET("persistence", FloatAttribute),
            GET("lacunarity", FloatAttribute),
            pa_ctrl,
            pa_dx,
            pa_dy,
            pa_angle,
            bbox);
      },
      p_node->get_config_ref()->hmap_transform_mode_gpu);

  // post-process
  post_apply_enveloppe(p_node, *p_out, p_env);

  post_process_heightmap(p_node,
                         *p_out,
                         GET("inverse", BoolAttribute),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_MEMBER("remap", RangeAttribute, is_active),
                         GET("remap", RangeAttribute));

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
