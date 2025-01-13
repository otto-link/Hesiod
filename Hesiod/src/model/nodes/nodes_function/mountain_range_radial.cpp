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
  p_node->add_attr<WaveNbAttribute>("kw",
                                    std::vector<float>(2, 4.f),
                                    0.f,
                                    32.f,
                                    true,
                                    "kw");
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<FloatAttribute>("half_width", 0.2f, 0.f, 1.f, "half_width");
  p_node->add_attr<FloatAttribute>("angle_spread_ratio",
                                   0.5f,
                                   0.f,
                                   1.f,
                                   "angle_spread_ratio");
  p_node->add_attr<FloatAttribute>("core_size_ratio",
                                   0.2f,
                                   0.01f,
                                   2.f,
                                   "core_size_ratio");
  p_node->add_attr<Vec2FloatAttribute>("center", "center");
  p_node->add_attr<IntAttribute>("octaves", 8, 0, 32, "octaves");
  p_node->add_attr<FloatAttribute>("weight", 0.7f, 0.f, 1.f, "weight");
  p_node->add_attr<FloatAttribute>("persistence", 0.5f, 0.f, 1.f, "persistence");
  p_node->add_attr<FloatAttribute>("lacunarity", 2.f, 0.01f, 4.f, "lacunarity");
  p_node->add_attr<BoolAttribute>("inverse", false, "inverse");
  p_node->add_attr<RangeAttribute>("remap_range", "remap_range");

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
                                "remap_range"});
}

void compute_mountain_range_radial_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

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
