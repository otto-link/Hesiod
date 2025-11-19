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

void setup_normal_displacement_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "radius", 0.05f, 0.f, 0.2f);
  ADD_ATTR(node, FloatAttribute, "amount", 5.f, 0.f, 20.f);
  ADD_ATTR(node, BoolAttribute, "reverse", false);
  ADD_ATTR(node, IntAttribute, "iterations", 3, 1, 10);
  ADD_ATTR(node, BoolAttribute, "GPU", HSD_DEFAULT_GPU_MODE);

  // attribute(s) order
  node.set_attr_ordered_key(
      {"radius", "amount", "reverse", "iterations", "_SEPARATOR_", "GPU"});
}

void compute_normal_displacement_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(0, (int)(GET(node, "radius", FloatAttribute) * p_in->shape.x));

    for (int it = 0; it < GET(node, "iterations", IntAttribute); it++)
    {
      if (GET(node, "GPU", BoolAttribute))
      {
        hmap::transform(
            {p_out, p_mask},
            [&node, &ir](std::vector<hmap::Array *> p_arrays)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::Array *pa_mask = p_arrays[1];

              hmap::gpu::normal_displacement(*pa_out,
                                             pa_mask,
                                             GET(node, "amount", FloatAttribute),
                                             ir,
                                             GET(node, "reverse", BoolAttribute));
            },
            node.get_config_ref()->hmap_transform_mode_gpu);
      }
      else
      {
        hmap::transform(
            {p_out, p_mask},
            [&node, &ir](std::vector<hmap::Array *> p_arrays)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::Array *pa_mask = p_arrays[1];

              hmap::normal_displacement(*pa_out,
                                        pa_mask,
                                        GET(node, "amount", FloatAttribute),
                                        ir,
                                        GET(node, "reverse", BoolAttribute));
            },
            node.get_config_ref()->hmap_transform_mode_cpu);
      }
      p_out->smooth_overlap_buffers();
    }
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
