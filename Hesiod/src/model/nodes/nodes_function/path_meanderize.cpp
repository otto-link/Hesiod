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

void setup_path_meanderize_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Path>(gnode::PortType::IN, "input");
  node.add_port<hmap::Path>(gnode::PortType::OUT, "output");

  // attribute(s)
  node.add_attr<FloatAttribute>("ratio", "ratio", 0.2f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("noise_ratio", "noise_ratio", 0.1f, 0.f, 1.f);

  node.add_attr<SeedAttribute>("seed", "seed");

  node.add_attr<IntAttribute>("iterations", "iterations", 2, 1, 8);

  node.add_attr<IntAttribute>("edge_divisions", "edge_divisions", 10, 1, 32);

  // attribute(s) order
  node.set_attr_ordered_key(
      {"ratio", "noise_ratio", "seed", "iterations", "edge_divisions"});
}

void compute_path_meanderize_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Path *p_in = node.get_value_ref<hmap::Path>("input");

  if (p_in)
  {
    hmap::Path *p_out = node.get_value_ref<hmap::Path>("output");

    // copy the input heightmap
    *p_out = *p_in;

    if (p_in->get_npoints() > 1)
      p_out->meanderize(node.get_attr<FloatAttribute>("ratio"),
                        node.get_attr<FloatAttribute>("noise_ratio"),
                        node.get_attr<SeedAttribute>("seed"),
                        node.get_attr<IntAttribute>("iterations"),
                        node.get_attr<IntAttribute>("edge_divisions"));
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
