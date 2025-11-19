/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/kernels.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_mean_shift_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "radius", 0.05f, 0.01f, 0.2f);
  ADD_ATTR(node, FloatAttribute, "talus_global", 16.f, 0.f, FLT_MAX);
  ADD_ATTR(node, IntAttribute, "iterations", 4, 1, 10);
  ADD_ATTR(node, BoolAttribute, "talus_weighted", true);

  // attribute(s) order
  node.set_attr_ordered_key({"radius", "talus_global", "iterations", "talus_weighted"});

  setup_pre_process_mask_attributes(node);
  setup_post_process_heightmap_attributes(node, true);
}

void compute_mean_shift_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // prepare mask
    std::shared_ptr<hmap::Heightmap> sp_mask = pre_process_mask(node, p_mask, *p_in);

    int   ir = std::max(1, (int)(GET(node, "radius", FloatAttribute) * p_out->shape.x));
    float talus = GET(node, "talus_global", FloatAttribute) / (float)p_out->shape.x;

    hmap::transform(
        {p_out, p_in, p_mask},
        [&node, ir, talus](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];
          hmap::Array *pa_mask = p_arrays[2];

          *pa_out = hmap::gpu::mean_shift(*pa_in,
                                          ir,
                                          talus,
                                          pa_mask,
                                          GET(node, "iterations", IntAttribute),
                                          GET(node, "talus_weighted", BoolAttribute));
        },
        node.get_config_ref()->hmap_transform_mode_gpu);

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(node, *p_out, p_in);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
