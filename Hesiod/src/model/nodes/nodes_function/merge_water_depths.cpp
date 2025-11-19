/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/hydrology.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_merge_water_depths_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "depth1");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "depth2");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_depth", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "k_smooth", 0.f, 0.f, 0.1f, "{:.4f}");

  // attribute(s) order
  node.set_attr_ordered_key({"k_smooth"});
}

void compute_merge_water_depths_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in1 = node.get_value_ref<hmap::Heightmap>("depth1");
  hmap::Heightmap *p_in2 = node.get_value_ref<hmap::Heightmap>("depth2");

  if (p_in1 && p_in2)
  {
    hmap::Heightmap *p_depth = node.get_value_ref<hmap::Heightmap>("water_depth");

    hmap::transform(
        {p_depth, p_in1, p_in2},
        [&node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_depth = p_arrays[0];
          hmap::Array *pa_in1 = p_arrays[1];
          hmap::Array *pa_in2 = p_arrays[2];

          *pa_depth = hmap::merge_water_depths(*pa_in1,
                                               *pa_in2,
                                               GET(node, "k_smooth", FloatAttribute));
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    p_depth->smooth_overlap_buffers();
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
