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
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "input");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "mask");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "output", CONFIG(node));
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "deposition", CONFIG(node));

  // attribute(s)
  std::vector<std::string> choices =
      {"Standard", "Linear", "Bedrock", "Olsen", "Ridge", "Inflate"};
  node.add_attr<ChoiceAttribute>("type", "", choices);

  node.add_attr<FloatAttribute>("talus_global", "Slope", 1.f, 0.f, FLT_MAX);
  node.add_attr<FloatAttribute>("duration", "Duration", 0.3f, 0.05f, 6.f);
  node.add_attr<BoolAttribute>("scale_talus_with_elevation",
                               "Scale with Elevation",
                               false);

  // attribute(s) order
  node.set_attr_ordered_key({
      "_GROUPBOX_BEGIN_Deposition Model",
      "type",
      "_GROUPBOX_END_",
      //
      "_GROUPBOX_BEGIN_Parameters",
      "talus_global",
      "scale_talus_with_elevation",
      "duration",
      "_GROUPBOX_END_",
  });

  setup_pre_process_mask_attributes(node);
  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = true, .remap_active_state = false});
}

void compute_thermal_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualArray *p_in = node.get_value_ref<hmap::VirtualArray>("input");

  if (p_in)
  {
    hmap::VirtualArray *p_mask = node.get_value_ref<hmap::VirtualArray>("mask");
    hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("output");
    hmap::VirtualArray *p_deposition_map = node.get_value_ref<hmap::VirtualArray>(
        "deposition");

    // prepare mask
    std::shared_ptr<hmap::VirtualArray> sp_mask = pre_process_mask(node, p_mask, *p_in);

    // inputs
    float talus = node.get_attr<FloatAttribute>("talus_global") / (float)p_out->shape.x;
    int   iterations = int(node.get_attr<FloatAttribute>("duration") * p_out->shape.x);

    hmap::VirtualArray talus_map = hmap::VirtualArray(CONFIG(node));
    talus_map.fill(talus, node.cfg().cm_cpu);

    if (node.get_attr<BoolAttribute>("scale_talus_with_elevation"))
    {
      talus_map.copy_from(*p_in, node.cfg().cm_cpu);
      talus_map.remap(talus / 100.f, talus, node.cfg().cm_cpu);
    }

    hmap::for_each_tile(
        {p_out, p_in, p_mask, &talus_map, p_deposition_map},
        [&node, talus, iterations](std::vector<hmap::Array *> p_arrays,
                                   const hmap::TileRegion &)
        {
          auto [pa_out, pa_in, pa_mask, pa_talus_map, pa_deposition_map] = unpack<5>(
              p_arrays);

          const std::string type = node.get_attr<ChoiceAttribute>("type");

          *pa_out = *pa_in;

          if (type == "Standard")
          {
            hmap::gpu::thermal(*pa_out,
                               pa_mask,
                               *pa_talus_map,
                               iterations,
                               nullptr, // bedrock
                               pa_deposition_map);
          }
          else if (type == "Ridge")
          {
            hmap::gpu::thermal_ridge(*pa_out,
                                     pa_mask,
                                     *pa_talus_map,
                                     iterations,
                                     pa_deposition_map);
          }
          else if (type == "Linear")
          {
            int iterations_half = int(0.5f * iterations);

            hmap::gpu::thermal(*pa_out,
                               pa_mask,
                               *pa_talus_map,
                               iterations_half,
                               /* pa_deposition_map */ nullptr);

            hmap::gpu::thermal_ridge(*pa_out,
                                     pa_mask,
                                     *pa_talus_map,
                                     iterations_half,
                                     pa_deposition_map);
          }
          else if (type == "Bedrock")
          {
            hmap::gpu::thermal_auto_bedrock(*pa_out,
                                            pa_mask,
                                            *pa_talus_map,
                                            iterations,
                                            pa_deposition_map);
          }
          else if (type == "Olsen")
          {
            hmap::gpu::thermal_olsen(*pa_out, pa_mask, *pa_talus_map, iterations);
            *pa_deposition_map = *pa_out - *pa_in;
          }
          else if (type == "Inflate")
          {
            hmap::gpu::thermal_inflate(*pa_out, pa_mask, *pa_talus_map, iterations);
            *pa_deposition_map = *pa_out - *pa_in;
          }
        },
        node.cfg().cm_gpu);

    // post-process
    p_out->smooth_overlap_buffers();
    post_process_heightmap(node, *p_out, p_in);

    if (p_deposition_map)
    {
      p_deposition_map->smooth_overlap_buffers();
      p_deposition_map->remap(0.f, 1.f, node.cfg().cm_cpu);
    }
  }
}

} // namespace hesiod
