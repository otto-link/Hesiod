/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_cloud_random_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");

  // attribute(s)
  p_node->add_attr<IntAttribute>("npoints", 50, 1, 1000, "npoints");
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<RangeAttribute>("remap_range", "remap_range");

  // attribute(s) order
  p_node->set_attr_ordered_key({"npoints", "seed", "_SEPARATOR_", "remap_range"});
}

void compute_cloud_random_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Cloud *p_out = p_node->get_value_ref<hmap::Cloud>("cloud");

  *p_out = hmap::Cloud(GET("npoints", IntAttribute), GET("seed", SeedAttribute));

  if (GET_ATTR("remap_range", RangeAttribute, is_active))
    p_out->remap_values(GET("remap_range", RangeAttribute)[0],
                        GET("remap_range", RangeAttribute)[1]);

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod