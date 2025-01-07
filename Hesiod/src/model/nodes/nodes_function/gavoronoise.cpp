/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_gavoronoise_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "angle");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<WaveNbAttribute>("kw");
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<FloatAttribute>("amplitude", 0.05f, 0.001f, 0.2f, "amplitude");
  p_node->add_attr<WaveNbAttribute>("kw_multiplier",
                                    std::vector<float>(2, 8.f),
                                    0.f,
                                    32.f,
                                    true,
                                    "kw_multiplier");
  p_node->add_attr<FloatAttribute>("angle", 0.f, -180.f, 180.f, "angle");
  p_node->add_attr<FloatAttribute>("angle_spread_ratio",
                                   1.f,
                                   0.f,
                                   1.f,
                                   "angle_spread_ratio");
  p_node->add_attr<FloatAttribute>("slope_strength", 0.5f, 0.f, 8.f, "slope_strength");
  p_node->add_attr<FloatAttribute>("branch_strength", 2.f, 0.f, 8.f, "branch_strength");
  p_node->add_attr<FloatAttribute>("z_cut_min", 0.2f, -1.f, 2.f, "z_cut_min");
  p_node->add_attr<FloatAttribute>("z_cut_max", 1.f, -1.f, 2.f, "z_cut_max");
  p_node->add_attr<IntAttribute>("octaves", 8, 0, 32, "Octaves");
  p_node->add_attr<FloatAttribute>("persistence", 0.4f, 0.f, 1.f, "Persistence");
  p_node->add_attr<FloatAttribute>("lacunarity", 2.f, 0.01f, 4.f, "Lacunarity");
  p_node->add_attr<BoolAttribute>("inverse", false, "Inverse");
  p_node->add_attr<RangeAttribute>("remap_range", "Remap range");

  // attribute(s) order
  p_node->set_attr_ordered_key({"kw",
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
                                "remap_range"});
}

void compute_gavoronoise_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

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

        hmap::Array angle_deg(shape, GET("angle", FloatAttribute));

        if (pa_angle)
          angle_deg += (*pa_angle) * 180.f / M_PI;

        *pa_out = hmap::gpu::gavoronoise(shape,
                                         GET("kw", WaveNbAttribute),
                                         GET("seed", SeedAttribute),
                                         angle_deg,
                                         GET("amplitude", FloatAttribute),
                                         GET("angle_spread_ratio", FloatAttribute),
                                         GET("kw_multiplier", WaveNbAttribute),
                                         GET("slope_strength", FloatAttribute),
                                         GET("branch_strength", FloatAttribute),
                                         GET("z_cut_min", FloatAttribute),
                                         GET("z_cut_max", FloatAttribute),
                                         GET("octaves", IntAttribute),
                                         GET("persistence", FloatAttribute),
                                         GET("lacunarity", FloatAttribute),
                                         pa_ctrl,
                                         pa_dx,
                                         pa_dy,
                                         bbox);
      },
      hmap::TransformMode::DISTRIBUTED);

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
  post_process_heightmap(*p_out,
                         GET("inverse", BoolAttribute),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_ATTR("remap_range", RangeAttribute, is_active),
                         GET("remap_range", RangeAttribute));

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
