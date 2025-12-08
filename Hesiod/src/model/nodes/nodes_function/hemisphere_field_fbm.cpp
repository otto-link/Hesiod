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

void setup_hemisphere_field_fbm_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dr");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "density");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "size");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<WaveNbAttribute>("kw", "Spatial Frequency");
  node.add_attr<SeedAttribute>("seed", "Seed");
  node.add_attr<FloatAttribute>("rmin", "rmin", 0.05f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("rmax", "rmax", 0.8f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("amplitude_random_ratio",
                                "amplitude_random_ratio",
                                0.5f,
                                0.f,
                                1.f);
  node.add_attr<FloatAttribute>("shift", "shift", 0.1f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("density", "density", 0.1f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("jitter.x", "jitter.x", 1.f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("jitter.y", "jitter.y", 1.f, 0.f, 1.f);
  node.add_attr<IntAttribute>("octaves", "Octaves", 8, 0, 32);
  node.add_attr<FloatAttribute>("persistence", "Persistence", 0.5f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("lacunarity", "Lacunarity", 2.f, 0.01f, 4.f);

  // attribute(s) order
  node.set_attr_ordered_key({"kw",
                             "seed",
                             "rmin",
                             "rmax",
                             "amplitude_random_ratio",
                             "shift",
                             "density",
                             "jitter.x",
                             "jitter.y",
                             "_SEPARATOR_",
                             "octaves",
                             "persistence",
                             "lacunarity"});

  setup_post_process_heightmap_attributes(node);
}

void compute_hemisphere_field_fbm_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_dr = node.get_value_ref<hmap::Heightmap>("dr");
  hmap::Heightmap *p_density = node.get_value_ref<hmap::Heightmap>("density");
  hmap::Heightmap *p_size = node.get_value_ref<hmap::Heightmap>("size");
  hmap::Heightmap *p_env = node.get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  hmap::transform(
      {p_out, p_dx, p_dy, p_dr, p_density, p_size},
      [&node](std::vector<hmap::Array *> p_arrays,
              hmap::Vec2<int>            shape,
              hmap::Vec4<float>          bbox)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::Array *pa_dx = p_arrays[1];
        hmap::Array *pa_dy = p_arrays[2];
        hmap::Array *pa_dr = p_arrays[3];
        hmap::Array *pa_density = p_arrays[4];
        hmap::Array *pa_size = p_arrays[5];

        hmap::Vec2<float> jitter(node.get_attr<FloatAttribute>("jitter.x"),
                                 node.get_attr<FloatAttribute>("jitter.y"));

        *pa_out = hmap::gpu::hemisphere_field_fbm(
            shape,
            node.get_attr<WaveNbAttribute>("kw"),
            node.get_attr<SeedAttribute>("seed"),
            node.get_attr<FloatAttribute>("rmin"),
            node.get_attr<FloatAttribute>("rmax"),
            node.get_attr<FloatAttribute>("amplitude_random_ratio"),
            node.get_attr<FloatAttribute>("density"),
            jitter,
            node.get_attr<FloatAttribute>("shift"),
            node.get_attr<IntAttribute>("octaves"),
            node.get_attr<FloatAttribute>("persistence"),
            node.get_attr<FloatAttribute>("lacunarity"),
            pa_dx,
            pa_dy,
            pa_dr,
            pa_density,
            pa_size,
            bbox);
      },
      node.get_config_ref()->hmap_transform_mode_gpu);

  // post-process
  post_apply_enveloppe(node, *p_out, p_env);
  post_process_heightmap(node, *p_out);
}

} // namespace hesiod
