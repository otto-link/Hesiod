/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_thermal_scree_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "zmax");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("talus_global", "Slope", 2.f, 0.f, FLT_MAX);
  node.add_attr<FloatAttribute>("zmax", "Scree Max Elevation", 0.5f, -1.f, 2.f);
  node.add_attr<FloatAttribute>("duration", "Duration", 0.3f, 0.05f, 6.f);
  // node.add_attr<BoolAttribute>("talus_constraint", "Talus Constraint", true);
  node.add_attr<BoolAttribute>("scale_talus_with_elevation",
                               "Scale with Elevation",
                               true);

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Slope Constraints",
                             "talus_global",
                             "scale_talus_with_elevation",
                             "zmax",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Deposition Dynamics",
                             "duration",
                             "_GROUPBOX_END_"});

  setup_pre_process_mask_attributes(node);
  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = true, .remap_active_state = false});
}

void compute_thermal_scree_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_zmax = node.get_value_ref<hmap::Heightmap>("zmax");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_deposition_map = node.get_value_ref<hmap::Heightmap>("deposition");

    // prepare mask
    std::shared_ptr<hmap::Heightmap> sp_mask = pre_process_mask(node, p_mask, *p_in);

    // copy the input heightmap
    *p_out = *p_in;

    float talus = node.get_attr<FloatAttribute>("talus_global") / (float)p_out->shape.x;
    int   iterations = int(node.get_attr<FloatAttribute>("duration") * p_out->shape.x);

    hmap::Heightmap talus_map = hmap::Heightmap(CONFIG(node), talus);

    if (node.get_attr<BoolAttribute>("scale_talus_with_elevation"))
    {
      talus_map = *p_in;
      talus_map.remap(talus / 10.f, talus);
    }

    hmap::Heightmap zmax = hmap::Heightmap();

    if (!p_zmax)
    {
      zmax = hmap::Heightmap(CONFIG(node), node.get_attr<FloatAttribute>("zmax"));
      p_zmax = &zmax;
    }

    hmap::transform(
        {p_out, p_mask, &talus_map, p_zmax, p_deposition_map},
        [&node, talus, iterations](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_mask = p_arrays[1];
          hmap::Array *pa_talus_map = p_arrays[2];
          hmap::Array *pa_zmax = p_arrays[3];
          hmap::Array *pa_deposition_map = p_arrays[4];

          hmap::gpu::thermal_scree(*pa_out,
                                   pa_mask,
                                   *pa_talus_map,
                                   *pa_zmax,
                                   iterations,
                                   pa_deposition_map);
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
