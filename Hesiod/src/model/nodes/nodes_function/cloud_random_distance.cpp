/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_cloud_random_distance_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "density");
  p_node->add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");

  // attribute(s)
  ADD_ATTR(FloatAttribute, "distance_min", 0.05f, 0.001f, 0.2f);
  ADD_ATTR(FloatAttribute, "distance_max", 0.1f, 0.001f, 1.f);
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(RangeAttribute, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"distance_min", "distance_max", "seed", "_SEPARATOR_", "remap"});
}

void compute_cloud_random_distance_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_density = p_node->get_value_ref<hmap::Heightmap>("density");
  hmap::Cloud     *p_out = p_node->get_value_ref<hmap::Cloud>("cloud");

  if (p_density)
  {
    // TODO distribute
    hmap::Array density_array = p_density->to_array();

    *p_out = hmap::random_cloud_distance(GET("distance_min", FloatAttribute),
                                         GET("distance_max", FloatAttribute),
                                         density_array,
                                         GET("seed", SeedAttribute));
  }
  else
  {
    *p_out = hmap::random_cloud_distance(GET("distance_min", FloatAttribute),
                                         GET("seed", SeedAttribute));
  }

  if (GET_MEMBER("remap", RangeAttribute, is_active))
    p_out->remap_values(GET("remap", RangeAttribute)[0], GET("remap", RangeAttribute)[1]);

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
