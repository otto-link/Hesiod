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

void setup_cloud_random_density_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "density");
  node.add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");

  // attribute(s)
  node.add_attr<IntAttribute>("npoints", "npoints", 50, 1, INT_MAX);

  node.add_attr<SeedAttribute>("seed", "seed");

  node.add_attr<RangeAttribute>("remap", "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"npoints", "seed", "_SEPARATOR_", "remap"});
}

void compute_cloud_random_density_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_density = node.get_value_ref<hmap::Heightmap>("density");

  if (p_density)
  {
    hmap::Cloud *p_out = node.get_value_ref<hmap::Cloud>("cloud");

    // TODO distribute
    hmap::Array density_array = p_density->to_array();

    *p_out = hmap::random_cloud_density(node.get_attr<IntAttribute>("npoints"),
                                        density_array,
                                        node.get_attr<SeedAttribute>("seed"));

    if (node.get_attr_ref<RangeAttribute>("remap")->get_is_active())
      p_out->remap_values(node.get_attr<RangeAttribute>("remap")[0],
                          node.get_attr<RangeAttribute>("remap")[1]);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
