/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_noise_fbm_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<EnumAttribute>("noise_type", "Type", enum_mappings.noise_type_map_fbm);
  node.add_attr<WaveNbAttribute>("kw", "Spatial Frequency");
  node.add_attr<SeedAttribute>("seed", "Seed");
  node.add_attr<IntAttribute>("octaves", "Octaves", 8, 0, 32);
  node.add_attr<FloatAttribute>("weight", "Weight", 0.7f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("persistence", "Persistence", 0.5f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("lacunarity", "Lacunarity", 2.f, 0.01f, 4.f);
  node.add_attr<BoolAttribute>("GPU", "GPU", HSD_DEFAULT_GPU_MODE);

  // attribute(s) order
  node.set_attr_ordered_key({"noise_type",
                             "kw",
                             "seed",
                             "octaves",
                             "weight",
                             "persistence",
                             "lacunarity",
                             "GPU"});

  setup_post_process_heightmap_attributes(node);
}

void compute_noise_fbm_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_ctrl = node.get_value_ref<hmap::Heightmap>("control");
  hmap::Heightmap *p_env = node.get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  if (node.get_attr<BoolAttribute>("GPU"))
  {
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

          *pa_out = hmap::gpu::noise_fbm(
              (hmap::NoiseType)node.get_attr<EnumAttribute>("noise_type"),
              shape,
              node.get_attr<WaveNbAttribute>("kw"),
              node.get_attr<SeedAttribute>("seed"),
              node.get_attr<IntAttribute>("octaves"),
              node.get_attr<FloatAttribute>("weight"),
              node.get_attr<FloatAttribute>("persistence"),
              node.get_attr<FloatAttribute>("lacunarity"),
              pa_ctrl,
              pa_dx,
              pa_dy,
              nullptr,
              bbox);
        },
        node.get_config_ref()->hmap_transform_mode_gpu);
  }
  else
  {
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

          *pa_out = hmap::noise_fbm(
              (hmap::NoiseType)node.get_attr<EnumAttribute>("noise_type"),
              shape,
              node.get_attr<WaveNbAttribute>("kw"),
              node.get_attr<SeedAttribute>("seed"),
              node.get_attr<IntAttribute>("octaves"),
              node.get_attr<FloatAttribute>("weight"),
              node.get_attr<FloatAttribute>("persistence"),
              node.get_attr<FloatAttribute>("lacunarity"),
              pa_ctrl,
              pa_dx,
              pa_dy,
              nullptr,
              bbox);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);
  }

  // post-process
  post_apply_enveloppe(node, *p_out, p_env);
  post_process_heightmap(node, *p_out);
}

} // namespace hesiod
