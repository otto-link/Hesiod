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

void setup_badlands_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "dx");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "dy");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "out", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("elevation", "elevation", 0.4f, 0.f, 1.f);
  node.add_attr<WaveNbAttribute>("kw", "Spatial Frequency");
  node.add_attr<SeedAttribute>("seed", "Seed");
  node.add_attr<IntAttribute>("octaves", "Octaves", 8, 0, 32);
  node.add_attr<FloatAttribute>("rugosity", "rugosity", 0.2f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("angle", "angle", 30.f, -180.f, 180.f);
  node.add_attr<FloatAttribute>("k_smoothing", "k_smoothing", 0.1f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("base_noise_amp", "base_noise_amp", 0.2f, 0.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"elevation",
                             "kw",
                             "seed",
                             "octaves",
                             "_SEPARATOR_",
                             "rugosity",
                             "angle",
                             "k_smoothing",
                             "base_noise_amp"});

  setup_post_process_heightmap_attributes(
      node,
      {.add_mix = false, .remap_active_state = false});
}

void compute_badlands_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base badlands function
  hmap::VirtualArray *p_dx = node.get_value_ref<hmap::VirtualArray>("dx");
  hmap::VirtualArray *p_dy = node.get_value_ref<hmap::VirtualArray>("dy");
  hmap::VirtualArray *p_env = node.get_value_ref<hmap::VirtualArray>("envelope");
  hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("out");

  hmap::for_each_tile(
      {p_out, p_dx, p_dy},
      [&node](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &region)
      {
        auto [pa_out, pa_dx, pa_dy] = unpack<3>(p_arrays);

        *pa_out = hmap::gpu::badlands(region.shape,
                                      node.get_attr<WaveNbAttribute>("kw"),
                                      node.get_attr<SeedAttribute>("seed"),
                                      node.get_attr<IntAttribute>("octaves"),
                                      node.get_attr<FloatAttribute>("rugosity"),
                                      node.get_attr<FloatAttribute>("angle"),
                                      node.get_attr<FloatAttribute>("k_smoothing"),
                                      node.get_attr<FloatAttribute>("base_noise_amp"),
                                      pa_dx,
                                      pa_dy,
                                      region.bbox);
      },
      node.cfg().cm_gpu);

  p_out->remap(0.f, node.get_attr<FloatAttribute>("elevation"), node.cfg().cm_cpu);

  // post-process
  post_apply_enveloppe(node, *p_out, p_env);
  post_process_heightmap(node, *p_out);
}

} // namespace hesiod
