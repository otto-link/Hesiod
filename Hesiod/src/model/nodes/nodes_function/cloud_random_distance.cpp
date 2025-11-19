/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_cloud_random_distance_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "density");
  node.add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "distance_min", 0.05f, 0.001f, 0.2f);
  ADD_ATTR(node, FloatAttribute, "distance_max", 0.1f, 0.001f, 1.f);
  ADD_ATTR(node, SeedAttribute, "seed");
  ADD_ATTR(node, RangeAttribute, "remap");

  // attribute(s) order
  node.set_attr_ordered_key(
      {"distance_min", "distance_max", "seed", "_SEPARATOR_", "remap"});
}

void compute_cloud_random_distance_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_density = node.get_value_ref<hmap::Heightmap>("density");
  hmap::Cloud     *p_out = node.get_value_ref<hmap::Cloud>("cloud");

  if (p_density)
  {
    // TODO distribute
    hmap::Array density_array = p_density->to_array();

    *p_out = hmap::random_cloud_distance(GET(node, "distance_min", FloatAttribute),
                                         GET(node, "distance_max", FloatAttribute),
                                         density_array,
                                         GET(node, "seed", SeedAttribute));
  }
  else
  {
    *p_out = hmap::random_cloud_distance(GET(node, "distance_min", FloatAttribute),
                                         GET(node, "seed", SeedAttribute));
  }

  if (GET_MEMBER(node, "remap", RangeAttribute, is_active))
    p_out->remap_values(GET(node, "remap", RangeAttribute)[0],
                        GET(node, "remap", RangeAttribute)[1]);

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
