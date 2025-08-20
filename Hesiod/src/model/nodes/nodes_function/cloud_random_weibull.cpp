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

void setup_cloud_random_weibull_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");

  // attribute(s)
  ADD_ATTR(FloatAttribute, "distance_min", 0.01f, 0.001f, 0.2f);
  ADD_ATTR(FloatAttribute, "lambda", 0.1f, 0.001f, 1.f);
  ADD_ATTR(FloatAttribute, "k", 1.5f, 0.01f, 4.f);
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(RangeAttribute, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"distance_min", "lambda", "k", "seed", "_SEPARATOR_", "remap"});
}

void compute_cloud_random_weibull_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Cloud *p_out = p_node->get_value_ref<hmap::Cloud>("cloud");

  *p_out = hmap::random_cloud_distance_weibull(GET("distance_min", FloatAttribute),
                                               GET("lambda", FloatAttribute),
                                               GET("k", FloatAttribute),
                                               GET("seed", SeedAttribute));

  if (GET_MEMBER("remap", RangeAttribute, is_active))
    p_out->remap_values(GET("remap", RangeAttribute)[0], GET("remap", RangeAttribute)[1]);

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
