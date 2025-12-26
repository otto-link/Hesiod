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
  std::vector<std::string> choices = {"Standard", "Bedrock", "Ridge", "Inflate"};
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

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_deposition_map = node.get_value_ref<hmap::Heightmap>("deposition");

    // prepare mask
    std::shared_ptr<hmap::Heightmap> sp_mask = pre_process_mask(node, p_mask, *p_in);

    // inputs
    float talus = node.get_attr<FloatAttribute>("talus_global") / (float)p_out->shape.x;
    int   iterations = int(node.get_attr<FloatAttribute>("duration") * p_out->shape.x);

    hmap::Heightmap talus_map = hmap::Heightmap(CONFIG(node), talus);

    if (node.get_attr<BoolAttribute>("scale_talus_with_elevation"))
    {
      talus_map = *p_in;
      talus_map.remap(talus / 100.f, talus);
    }

    hmap::transform(
        {p_out, p_in, p_mask, &talus_map, p_deposition_map},
        [&node, talus, iterations](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];
          hmap::Array *pa_mask = p_arrays[2];
          hmap::Array *pa_talus_map = p_arrays[3];
          hmap::Array *pa_deposition_map = p_arrays[4];

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
          else if (type == "Bedrock")
          {
            hmap::gpu::thermal_auto_bedrock(*pa_out,
                                            pa_mask,
                                            *pa_talus_map,
                                            iterations,
                                            pa_deposition_map);
          }
          else if (type == "Inflate")
          {
            hmap::gpu::thermal_inflate(*pa_out, pa_mask, *pa_talus_map, iterations);
            *pa_deposition_map = 0.f;
          }
        },
        node.get_config_ref()->hmap_transform_mode_gpu);

    // post-process
    p_out->smooth_overlap_buffers();
    post_process_heightmap(node, *p_out, p_in);

    if (p_deposition_map)
    {
      p_deposition_map->smooth_overlap_buffers();
      p_deposition_map->remap();
    }
  }
}

} // namespace hesiod
