/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_smooth_fill_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");

  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("radius", "radius", 0.05f, 0.001f, 0.2f);

  node.add_attr<FloatAttribute>("k", "k", 0.01f, 0.01f, 1.f);

  node.add_attr<BoolAttribute>("normalized_map", "normalized_map", true);

  // attribute(s) order
  node.set_attr_ordered_key({"radius", "k", "normalized_map"});

  setup_pre_process_mask_attributes(node);
  setup_post_process_heightmap_attributes(node, true);
}

void compute_smooth_fill_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  Q_EMIT node.compute_started(node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_deposition_map = node.get_value_ref<hmap::Heightmap>("deposition");

    // prepare mask
    std::shared_ptr<hmap::Heightmap> sp_mask = pre_process_mask(node, p_mask, *p_in);

    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(node.get_attr<FloatAttribute>("radius") * p_out->shape.x));

    hmap::transform(
        {p_out, p_mask, p_deposition_map},
        [&node, &ir](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_mask = p_arrays[1];
          hmap::Array *pa_deposition = p_arrays[2];

          hmap::gpu::smooth_fill(*pa_out,
                                 ir,
                                 pa_mask,
                                 node.get_attr<FloatAttribute>("k"),
                                 pa_deposition);
        },
        node.get_config_ref()->hmap_transform_mode_gpu);

    p_out->smooth_overlap_buffers();
    p_deposition_map->smooth_overlap_buffers();

    if (node.get_attr<BoolAttribute>("normalized_map"))
      p_deposition_map->remap();

    // post-process
    post_process_heightmap(node, *p_out, p_in);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
