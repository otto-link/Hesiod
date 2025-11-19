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

void setup_gavoronoise_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "angle");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, WaveNbAttribute, "kw");
  ADD_ATTR(node, SeedAttribute, "seed");
  ADD_ATTR(node, FloatAttribute, "amplitude", 0.05f, 0.001f, 0.2f);
  ADD_ATTR(node,
           WaveNbAttribute,
           "kw_multiplier",
           std::vector<float>(2, 8.f),
           0.f,
           32.f,
           true);
  ADD_ATTR(node, FloatAttribute, "angle", 0.f, -180.f, 180.f);
  ADD_ATTR(node, FloatAttribute, "angle_spread_ratio", 1.f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "slope_strength", 0.5f, 0.f, 8.f);
  ADD_ATTR(node, FloatAttribute, "branch_strength", 2.f, 0.f, 8.f);
  ADD_ATTR(node, FloatAttribute, "z_cut_min", 0.2f, -1.f, 2.f);
  ADD_ATTR(node, FloatAttribute, "z_cut_max", 1.f, -1.f, 2.f);
  ADD_ATTR(node, IntAttribute, "octaves", 8, 0, 32);
  ADD_ATTR(node, FloatAttribute, "persistence", 0.4f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "lacunarity", 2.f, 0.01f, 4.f);
  ADD_ATTR(node, BoolAttribute, "inverse", false);
  ADD_ATTR(node, RangeAttribute, "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"kw",
                             "seed",
                             "amplitude",
                             "kw_multiplier",
                             "angle",
                             "angle_spread_ratio",
                             "slope_strength",
                             "branch_strength",
                             "z_cut_min",
                             "z_cut_max",
                             "octaves",
                             "persistence",
                             "lacunarity",
                             "_SEPARATOR_",
                             "inverse",
                             "remap"});
}

void compute_gavoronoise_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

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

        hmap::Array angle_deg(shape, GET(node, "angle", FloatAttribute));

        if (pa_angle)
          angle_deg += (*pa_angle) * 180.f / M_PI;

        *pa_out = hmap::gpu::gavoronoise(shape,
                                         GET(node, "kw", WaveNbAttribute),
                                         GET(node, "seed", SeedAttribute),
                                         angle_deg,
                                         GET(node, "amplitude", FloatAttribute),
                                         GET(node, "angle_spread_ratio", FloatAttribute),
                                         GET(node, "kw_multiplier", WaveNbAttribute),
                                         GET(node, "slope_strength", FloatAttribute),
                                         GET(node, "branch_strength", FloatAttribute),
                                         GET(node, "z_cut_min", FloatAttribute),
                                         GET(node, "z_cut_max", FloatAttribute),
                                         GET(node, "octaves", IntAttribute),
                                         GET(node, "persistence", FloatAttribute),
                                         GET(node, "lacunarity", FloatAttribute),
                                         pa_ctrl,
                                         pa_dx,
                                         pa_dy,
                                         bbox);
      },
      node.get_config_ref()->hmap_transform_mode_gpu);

  // add envelope
  if (p_env)
  {
    float hmin = p_out->min();
    hmap::transform(*p_out,
                    *p_env,
                    [&hmin](hmap::Array &a, hmap::Array &b)
                    {
                      a -= hmin;
                      a *= b;
                    });
  }

  // post-process
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
