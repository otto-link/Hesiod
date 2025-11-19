/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/morphology.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_relative_distance_from_skeleton_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("search_radius", "search_radius", 0.2f, 0.f, 0.5f);

  node.add_attr<FloatAttribute>("threshold", "threshold", 0.f, -1.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"search_radius", "threshold"});

  setup_post_process_heightmap_attributes(node);
}

void compute_relative_distance_from_skeleton_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    int ir = std::max(
        1,
        (int)(node.get_attr<FloatAttribute>("search_radius") * p_out->shape.x));

    hmap::transform(
        {p_out, p_in},
        [&node, ir](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = *pa_in;

          float threshold = node.get_attr<FloatAttribute>("threshold");
          if (threshold)
          {
            hmap::make_binary(*pa_out, threshold);
            *pa_out -= threshold;
          }

          *pa_out = hmap::gpu::relative_distance_from_skeleton(*pa_out, ir);
        },
        node.get_config_ref()->hmap_transform_mode_gpu);

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(node, *p_out);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
