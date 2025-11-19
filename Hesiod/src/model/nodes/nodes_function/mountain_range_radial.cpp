/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_mountain_range_radial_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "angle", CONFIG);

  // attribute(s)
  ADD_ATTR(node, WaveNbAttribute, "kw", std::vector<float>(2, 4.f), 0.f, FLT_MAX, true);
  ADD_ATTR(node, SeedAttribute, "seed");
  ADD_ATTR(node, FloatAttribute, "half_width", 0.2f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "angle_spread_ratio", 0.5f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "core_size_ratio", 0.2f, 0.01f, 2.f);
  ADD_ATTR(node, Vec2FloatAttribute, "center");
  ADD_ATTR(node, IntAttribute, "octaves", 8, 0, 32);
  ADD_ATTR(node, FloatAttribute, "weight", 0.7f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "persistence", 0.5f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "lacunarity", 2.f, 0.01f, 4.f);
  ADD_ATTR(node, BoolAttribute, "inverse", false);
  ADD_ATTR(node, RangeAttribute, "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"kw",
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

void compute_mountain_range_radial_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_ctrl = node.get_value_ref<hmap::Heightmap>("control");
  hmap::Heightmap *p_env = node.get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
  hmap::Heightmap *p_angle = node.get_value_ref<hmap::Heightmap>("angle");

  hmap::transform(
      {p_out, p_ctrl, p_dx, p_dy, p_angle},
      [&node](std::vector<hmap::Array *> p_arrays,
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
            GET(node, "kw", WaveNbAttribute),
            GET(node, "seed", SeedAttribute),
            GET(node, "half_width", FloatAttribute),
            GET(node, "angle_spread_ratio", FloatAttribute),
            GET(node, "core_size_ratio", FloatAttribute),
            GET(node, "center", Vec2FloatAttribute),
            GET(node, "octaves", IntAttribute),
            GET(node, "weight", FloatAttribute),
            GET(node, "persistence", FloatAttribute),
            GET(node, "lacunarity", FloatAttribute),
            pa_ctrl,
            pa_dx,
            pa_dy,
            pa_angle,
            bbox);
      },
      node.get_config_ref()->hmap_transform_mode_gpu);

  // post-process
  post_apply_enveloppe(node, *p_out, p_env);

  post_process_heightmap(node,
                         *p_out,
                         GET(node, "inverse", BoolAttribute),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_MEMBER(node, "remap", RangeAttribute, is_active),
                         GET(node, "remap", RangeAttribute));

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
