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
  node.add_attr<IntAttribute>("iterations", "iterations", 4, 1, 10);
  node.add_attr<SeedAttribute>("seed", "Seed");
  node.add_attr<FloatAttribute>("sigma", "sigma", 0.3f, 0.f, 1.f);
  node.add_attr<IntAttribute>("orientation", "orientation", 0, 0, 1);
  node.add_attr<FloatAttribute>("persistence", "Persistence", 1.f, 0.01f, 4.f);

  // attribute(s) order
  node.set_attr_ordered_key(
      {"iterations", "seed", "sigma", "orientation", "persistence"});
}

void compute_path_fractalize_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Path *p_in = node.get_value_ref<hmap::Path>("input");

  if (p_in)
  {
    hmap::Path *p_out = node.get_value_ref<hmap::Path>("output");

    // copy the input heightmap
    *p_out = *p_in;

    if (p_in->get_npoints() > 1)
      p_out->fractalize(node.get_attr<IntAttribute>("iterations"),
                        node.get_attr<SeedAttribute>("seed"),
                        node.get_attr<FloatAttribute>("sigma"),
                        node.get_attr<IntAttribute>("orientation"),
                        node.get_attr<FloatAttribute>("persistence"));
  }
}

} // namespace hesiod
