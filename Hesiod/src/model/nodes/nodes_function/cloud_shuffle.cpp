/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/cloud.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_cloud_shuffle_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Cloud>(gnode::PortType::IN, "input");
  node.add_port<hmap::Cloud>(gnode::PortType::OUT, "output");

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "dx", 0.f, -0.5f, 0.5f);
  ADD_ATTR(node, FloatAttribute, "dy", 0.f, -0.5f, 0.5f);
  ADD_ATTR(node, FloatAttribute, "dv", 0.f, -0.5f, 0.5f);
  ADD_ATTR(node, SeedAttribute, "seed");

  // attribute(s) order
  node.set_attr_ordered_key({"dx", "dy", "dv", "seed"});
}

void compute_cloud_shuffle_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Cloud *p_in = node.get_value_ref<hmap::Cloud>("input");

  if (p_in)
  {
    hmap::Cloud *p_out = node.get_value_ref<hmap::Cloud>("output");

    // copy the input heightmap
    *p_out = *p_in;

    if (p_in->get_npoints() > 0)
      p_out->shuffle(GET(node, "dx", FloatAttribute),
                     GET(node, "dy", FloatAttribute),
                     GET(node, "seed", SeedAttribute),
                     GET(node, "dv", FloatAttribute));
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
