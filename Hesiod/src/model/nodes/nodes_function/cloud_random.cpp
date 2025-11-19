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

void setup_cloud_random_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");

  // attribute(s)
  ADD_ATTR(node, IntAttribute, "npoints", 50, 1, INT_MAX);
  ADD_ATTR(node, SeedAttribute, "seed");
  ADD_ATTR(node,
           EnumAttribute,
           "method",
           hmap::point_sampling_method_as_string,
           "Latin Hypercube Sampling");
  ADD_ATTR(node, RangeAttribute, "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"npoints", "seed", "method", "_SEPARATOR_", "remap"});
}

void compute_cloud_random_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Cloud *p_out = node.get_value_ref<hmap::Cloud>("cloud");

  *p_out = hmap::random_cloud(
      GET(node, "npoints", IntAttribute),
      GET(node, "seed", SeedAttribute),
      (hmap::PointSamplingMethod)GET(node, "method", EnumAttribute));

  if (GET_MEMBER(node, "remap", RangeAttribute, is_active))
    p_out->remap_values(GET(node, "remap", RangeAttribute)[0],
                        GET(node, "remap", RangeAttribute)[1]);

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
