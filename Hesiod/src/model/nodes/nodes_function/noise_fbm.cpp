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

void setup_noise_fbm_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<MapEnumAttribute>("noise_type", noise_type_map_fbm, "Noise type");
  p_node->add_attr<WaveNbAttribute>("kw");
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<IntAttribute>("octaves", 8, 0, 32, "Octaves");
  p_node->add_attr<FloatAttribute>("weight", 0.7f, 0.f, 1.f, "Weight");
  p_node->add_attr<FloatAttribute>("persistence", 0.5f, 0.f, 1.f, "Persistence");
  p_node->add_attr<FloatAttribute>("lacunarity", 2.f, 0.01f, 4.f, "Lacunarity");
  p_node->add_attr<BoolAttribute>("inverse", false, "Inverse");
  p_node->add_attr<RangeAttribute>("remap_range", "Remap range");
  p_node->add_attr<BoolAttribute>("GPU", true, "GPU");

  // attribute(s) order
  p_node->set_attr_ordered_key({"noise_type",
                                "_SEPARATOR_",
                                "kw",
                                "seed",
                                "octaves",
                                "weight",
                                "persistence",
                                "lacunarity",
                                "_SEPARATOR_",
                                "inverse",
                                "remap_range",
                                "_SEPARATOR_",
                                "GPU"});
}

void compute_noise_fbm_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  // base noise function
  hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_ctrl = p_node->get_value_ref<hmap::Heightmap>("control");
  hmap::Heightmap *p_env = p_node->get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

  if (GET("GPU", BoolAttribute))
  {
    hmap::transform(
        {p_out, p_ctrl, p_dx, p_dy},
        [p_node](std::vector<hmap::Array *> p_arrays,
                 hmap::Vec2<int>            shape,
                 hmap::Vec4<float>          bbox)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_ctrl = p_arrays[1];
          hmap::Array *pa_dx = p_arrays[2];
          hmap::Array *pa_dy = p_arrays[3];

          *pa_out = hmap::gpu::noise_fbm(
              (hmap::NoiseType)GET("noise_type", MapEnumAttribute),
              shape,
              GET("kw", WaveNbAttribute),
              GET("seed", SeedAttribute),
              GET("octaves", IntAttribute),
              GET("weight", FloatAttribute),
              GET("persistence", FloatAttribute),
              GET("lacunarity", FloatAttribute),
              pa_ctrl,
              pa_dx,
              pa_dy,
              nullptr,
              bbox);
        },
        hmap::TransformMode::DISTRIBUTED);
  }
  else
  {
    hmap::transform(
        {p_out, p_ctrl, p_dx, p_dy},
        [p_node](std::vector<hmap::Array *> p_arrays,
                 hmap::Vec2<int>            shape,
                 hmap::Vec4<float>          bbox)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_ctrl = p_arrays[1];
          hmap::Array *pa_dx = p_arrays[2];
          hmap::Array *pa_dy = p_arrays[3];

          *pa_out = hmap::noise_fbm((hmap::NoiseType)GET("noise_type", MapEnumAttribute),
                                    shape,
                                    GET("kw", WaveNbAttribute),
                                    GET("seed", SeedAttribute),
                                    GET("octaves", IntAttribute),
                                    GET("weight", FloatAttribute),
                                    GET("persistence", FloatAttribute),
                                    GET("lacunarity", FloatAttribute),
                                    pa_ctrl,
                                    pa_dx,
                                    pa_dy,
                                    nullptr,
                                    bbox);
        },
        hmap::TransformMode::DISTRIBUTED);
  }

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
