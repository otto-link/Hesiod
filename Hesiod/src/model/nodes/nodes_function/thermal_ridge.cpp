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

void setup_thermal_ridge_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "talus_global", 2.f, 0.f, FLT_MAX);
  ADD_ATTR(IntAttribute, "iterations", 500, 1, INT_MAX);
  ADD_ATTR(BoolAttribute, "scale_talus_with_elevation", false);
  ADD_ATTR(RangeAttribute, "remap", false);

  // attribute(s) order
  p_node->set_attr_ordered_key({"talus_global",
                                "iterations",
                                "scale_talus_with_elevation",
                                "_SEPARATOR_",
                                "remap"});
}

void compute_thermal_ridge_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // AppContext &ctx = HSD_CTX;

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_deposition_map = p_node->get_value_ref<hmap::Heightmap>(
        "deposition");

    // copy the input heightmap
    *p_out = *p_in;

    float talus = GET("talus_global", FloatAttribute) / (float)p_out->shape.x;

    hmap::Heightmap talus_map = hmap::Heightmap(CONFIG, talus);

    if (GET("scale_talus_with_elevation", BoolAttribute))
    {
      talus_map = *p_in;
      talus_map.remap(talus / 10.f, talus);
    }

    hmap::transform(
        {p_out, p_mask, &talus_map, p_deposition_map},
        [p_node, &talus](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_mask = p_arrays[1];
          hmap::Array *pa_talus_map = p_arrays[2];
          hmap::Array *pa_deposition_map = p_arrays[3];

          hmap::gpu::thermal_ridge(*pa_out,
                                   pa_mask,
                                   *pa_talus_map,
                                   GET("iterations", IntAttribute),
                                   pa_deposition_map);
        },
        HSD_GPU_MODE);

    p_out->smooth_overlap_buffers();

    p_deposition_map->smooth_overlap_buffers();
    p_deposition_map->remap();

    // post-process
    post_process_heightmap(p_node,
                           *p_out,
                           false, // inverse
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_MEMBER("remap", RangeAttribute, is_active),
                           GET("remap", RangeAttribute));
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
