/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "highmap/dbg/timer.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_thermal_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("talus_global", "talus_global", 1.f, 0.f, FLT_MAX);
  node.add_attr<IntAttribute>("iterations", "iterations", 100, 1, INT_MAX);
  node.add_attr<BoolAttribute>("scale_talus_with_elevation",
                               "scale_talus_with_elevation",
                               false);
  node.add_attr<BoolAttribute>("GPU", "GPU", HSD_DEFAULT_GPU_MODE);

  // attribute(s) order
  node.set_attr_ordered_key(
      {"talus_global", "iterations", "scale_talus_with_elevation", "_SEPARATOR_", "GPU"});
}

void compute_thermal_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_deposition_map = node.get_value_ref<hmap::Heightmap>("deposition");

    // copy the input heightmap
    *p_out = *p_in;

    float talus = node.get_attr<FloatAttribute>("talus_global") / (float)p_out->shape.x;

    hmap::Heightmap talus_map = hmap::Heightmap(CONFIG(node), talus);

    if (node.get_attr<BoolAttribute>("scale_talus_with_elevation"))
    {
      talus_map = *p_in;
      talus_map.remap(talus / 100.f, talus);
    }

    if (node.get_attr<BoolAttribute>("GPU"))
    {
      hmap::transform(
          {p_out, p_mask, &talus_map, p_deposition_map},
          [&node, &talus](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];
            hmap::Array *pa_talus_map = p_arrays[2];
            hmap::Array *pa_deposition_map = p_arrays[3];

            hmap::gpu::thermal(*pa_out,
                               pa_mask,
                               *pa_talus_map,
                               node.get_attr<IntAttribute>("iterations"),
                               nullptr, // bedrock
                               pa_deposition_map);
          },
          node.get_config_ref()->hmap_transform_mode_gpu);
    }
    else
    {
      hmap::transform(
          {p_out, p_mask, &talus_map, p_deposition_map},
          [&node, &talus](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];
            hmap::Array *pa_talus_map = p_arrays[2];
            hmap::Array *pa_deposition_map = p_arrays[3];

            hmap::thermal(*pa_out,
                          pa_mask,
                          *pa_talus_map,
                          node.get_attr<IntAttribute>("iterations"),
                          nullptr, // bedrock
                          pa_deposition_map);
          },
          node.get_config_ref()->hmap_transform_mode_cpu);
    }

    p_out->smooth_overlap_buffers();

    if (p_deposition_map)
    {
      p_deposition_map->smooth_overlap_buffers();
      p_deposition_map->remap();
    }
  }
}

} // namespace hesiod
