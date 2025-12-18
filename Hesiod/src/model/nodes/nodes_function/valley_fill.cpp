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

void setup_valley_fill_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("talus_global", "Slope", 2.f, 0.f, FLT_MAX);
  node.add_attr<FloatAttribute>("duration", "Duration", 1.f, 0.05f, 6.f);
  node.add_attr<FloatAttribute>("ratio", "Deposition Ratio", 0.8f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("gamma", "Deposition Gamma", 2.f, 0.01f, 4.f);
  node.add_attr<BoolAttribute>("scale_talus_with_elevation",
                               "Scale with Elevation",
                               true);

  // attribute(s) order
  node.set_attr_ordered_key({
      "_GROUPBOX_BEGIN_Slope Constraints",
      "talus_global",
      "scale_talus_with_elevation",
      "_GROUPBOX_END_",
      //
      "_GROUPBOX_BEGIN_Deposition Profile",
      "ratio",
      "gamma",
      "_GROUPBOX_END_",
      //
      "_GROUPBOX_BEGIN_Deposition Dynamics",
      "duration",
      "_GROUPBOX_END_",
  });

  setup_post_process_heightmap_attributes(node, true, false);
}

void compute_valley_fill_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    float talus = node.get_attr<FloatAttribute>("talus_global") / (float)p_out->shape.x;
    int   iterations = int(node.get_attr<FloatAttribute>("duration") * p_out->shape.x);

    hmap::Heightmap talus_map = hmap::Heightmap(CONFIG(node), talus);

    if (node.get_attr<BoolAttribute>("scale_talus_with_elevation"))
    {
      talus_map = *p_in;
      talus_map.remap(talus / 100.f, talus);
    }

    float zmin = p_in->min();
    float zmax = p_in->max();

    hmap::transform(
        {p_out, p_in, p_mask, &talus_map},
        [&node, talus, iterations, zmin, zmax](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];
          hmap::Array *pa_mask = p_arrays[2];
          hmap::Array *pa_talus_map = p_arrays[3];

          *pa_out = *pa_in;

          hmap::gpu::valley_fill(*pa_out,
                                 pa_mask,
                                 *pa_talus_map,
                                 iterations,
                                 node.get_attr<FloatAttribute>("gamma"),
                                 node.get_attr<FloatAttribute>("ratio"),
                                 zmin,
                                 zmax);
        },
        node.get_config_ref()->hmap_transform_mode_gpu);

    // post-process
    p_out->smooth_overlap_buffers();
    post_process_heightmap(node, *p_out, p_in);
  }
}

} // namespace hesiod
