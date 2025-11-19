/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/grids.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_cloud_lattice_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");

  // attribute(s)
  std::vector<float> default_value = {0.1f, 0.1f};

  ADD_ATTR(node, IntAttribute, "npoints", 50, 1, INT_MAX);
  ADD_ATTR(node, WaveNbAttribute, "stagger_ratio", default_value, 0.f, 1.f, true);
  ADD_ATTR(node, WaveNbAttribute, "jitter_ratio", default_value, 0.f, 1.f, true);
  ADD_ATTR(node, SeedAttribute, "seed");
  ADD_ATTR(node, RangeAttribute, "remap");

  // attribute(s) order
  node.set_attr_ordered_key(
      {"npoints", "stagger_ratio", "jitter_ratio", "seed", "_SEPARATOR_", "remap"});
}

void compute_cloud_lattice_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Cloud *p_out = node.get_value_ref<hmap::Cloud>("cloud");

  *p_out = hmap::random_cloud_jittered(GET(node, "npoints", IntAttribute),
                                       GET(node, "jitter_ratio", WaveNbAttribute),
                                       GET(node, "stagger_ratio", WaveNbAttribute),
                                       GET(node, "seed", SeedAttribute));

  if (GET_MEMBER(node, "remap", RangeAttribute, is_active))
    p_out->remap_values(GET(node, "remap", RangeAttribute)[0],
                        GET(node, "remap", RangeAttribute)[1]);

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
