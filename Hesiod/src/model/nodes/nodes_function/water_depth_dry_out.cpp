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

void setup_water_depth_dry_out_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "depth");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_depth", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "dry_out_ratio", 0.1f, 0.f, 0.5f, "{:.4f}");

  // attribute(s) order
  node.set_attr_ordered_key({"dry_out_ratio"});
}

void compute_water_depth_dry_out_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("depth");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("water_depth");

    // copy the input heightmap
    *p_out = *p_in;

    float depth_max = p_in->max();

    hmap::transform(
        {p_out, p_mask},
        [&node, depth_max](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_mask = p_arrays[1];

          hmap::water_depth_dry_out(*pa_out,
                                    GET(node, "dry_out_ratio", FloatAttribute),
                                    pa_mask,
                                    depth_max);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
