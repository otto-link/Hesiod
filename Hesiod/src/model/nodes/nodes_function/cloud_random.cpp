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

void setup_cloud_random_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");

  // attribute(s)
  ADD_ATTR(IntAttribute, "npoints", 50, 1, INT_MAX);
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(EnumAttribute,
           "method",
           hmap::point_sampling_method_as_string,
           "Latin Hypercube Sampling");
  ADD_ATTR(RangeAttribute, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key({"npoints", "seed", "method", "_SEPARATOR_", "remap"});
}

void compute_cloud_random_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Cloud *p_out = p_node->get_value_ref<hmap::Cloud>("cloud");

  *p_out = hmap::random_cloud(GET("npoints", IntAttribute),
                              GET("seed", SeedAttribute),
                              (hmap::PointSamplingMethod)GET("method", EnumAttribute));

  if (GET_MEMBER("remap", RangeAttribute, is_active))
    p_out->remap_values(GET("remap", RangeAttribute)[0], GET("remap", RangeAttribute)[1]);

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
