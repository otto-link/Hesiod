/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_median3x3_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, BoolAttribute, "GPU", HSD_DEFAULT_GPU_MODE);
}

void compute_median3x3_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    if (GET(node, "GPU", BoolAttribute))
    {
      hmap::transform(
          {p_out, p_in, p_mask},
          [](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_in = p_arrays[1];
            hmap::Array *pa_mask = p_arrays[2];

            *pa_out = *pa_in;

            hmap::gpu::median_3x3(*pa_in, pa_mask);
          },
          node.get_config_ref()->hmap_transform_mode_gpu);
    }
    else
    {
      hmap::transform(
          {p_out, p_in, p_mask},
          [](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_in = p_arrays[1];
            hmap::Array *pa_mask = p_arrays[2];

            *pa_out = *pa_in;

            hmap::median_3x3(*pa_in, pa_mask);
          },
          node.get_config_ref()->hmap_transform_mode_gpu);
    }

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
