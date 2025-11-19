/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/path.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_path_fractalize_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Path>(gnode::PortType::IN, "input");
  node.add_port<hmap::Path>(gnode::PortType::OUT, "output");

  // attribute(s)
  ADD_ATTR(node, IntAttribute, "iterations", 4, 1, 10);
  ADD_ATTR(node, SeedAttribute, "seed");
  ADD_ATTR(node, FloatAttribute, "sigma", 0.3f, 0.f, 1.f);
  ADD_ATTR(node, IntAttribute, "orientation", 0, 0, 1);
  ADD_ATTR(node, FloatAttribute, "persistence", 1.f, 0.01f, 4.f);

  // attribute(s) order
  node.set_attr_ordered_key(
      {"iterations", "seed", "sigma", "orientation", "persistence"});
}

void compute_path_fractalize_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Path *p_in = node.get_value_ref<hmap::Path>("input");

  if (p_in)
  {
    hmap::Path *p_out = node.get_value_ref<hmap::Path>("output");

    // copy the input heightmap
    *p_out = *p_in;

    if (p_in->get_npoints() > 1)
      p_out->fractalize(GET(node, "iterations", IntAttribute),
                        GET(node, "seed", SeedAttribute),
                        GET(node, "sigma", FloatAttribute),
                        GET(node, "orientation", IntAttribute),
                        GET(node, "persistence", FloatAttribute));
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
