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
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<WaveNbAttribute>("kw", "kw");
  node.add_attr<SeedAttribute>("seed", "seed");
  node.add_attr<FloatAttribute>("amplitude", "amplitude", 0.05f, 0.001f, 0.2f);

  node.add_attr<WaveNbAttribute>("kw_multiplier",
                                 "kw_multiplier",
                                 std::vector<float>(2, 8.f),
                                 0.f,
                                 32.f,
                                 true);
  node.add_attr<FloatAttribute>("angle", "angle", 0.f, -180.f, 180.f);
  node.add_attr<FloatAttribute>("angle_spread_ratio",
                                "angle_spread_ratio",
                                1.f,
                                0.f,
                                1.f);
  node.add_attr<FloatAttribute>("slope_strength", "slope_strength", 0.5f, 0.f, 8.f);
  node.add_attr<FloatAttribute>("branch_strength", "branch_strength", 2.f, 0.f, 8.f);
  node.add_attr<FloatAttribute>("z_cut_min", "z_cut_min", 0.2f, -1.f, 2.f);
  node.add_attr<FloatAttribute>("z_cut_max", "z_cut_max", 1.f, -1.f, 2.f);
  node.add_attr<IntAttribute>("octaves", "octaves", 8, 0, 32);
  node.add_attr<FloatAttribute>("persistence", "persistence", 0.4f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("lacunarity", "lacunarity", 2.f, 0.01f, 4.f);
  node.add_attr<BoolAttribute>("inverse", "inverse", false);
  node.add_attr<RangeAttribute>("remap", "remap");

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
  if (node.compute_started)
    node.compute_finished(node.get_id());

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

        hmap::Array angle_deg(shape, node.get_attr<FloatAttribute>("angle"));

        if (pa_angle)
          angle_deg += (*pa_angle) * 180.f / M_PI;

        *pa_out = hmap::gpu::gavoronoise(
            shape,
            node.get_attr<WaveNbAttribute>("kw"),
            node.get_attr<SeedAttribute>("seed"),
            angle_deg,
            node.get_attr<FloatAttribute>("amplitude"),
            node.get_attr<FloatAttribute>("angle_spread_ratio"),
            node.get_attr<WaveNbAttribute>("kw_multiplier"),
            node.get_attr<FloatAttribute>("slope_strength"),
            node.get_attr<FloatAttribute>("branch_strength"),
            node.get_attr<FloatAttribute>("z_cut_min"),
            node.get_attr<FloatAttribute>("z_cut_max"),
            node.get_attr<IntAttribute>("octaves"),
            node.get_attr<FloatAttribute>("persistence"),
            node.get_attr<FloatAttribute>("lacunarity"),
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

                         node.get_attr<BoolAttribute>("inverse"),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         node.get_attr_ref<RangeAttribute>("remap")->get_is_active(),
                         node.get_attr<RangeAttribute>("remap"));

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
