/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_fill_talus_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "slope", 4.f, 0.1f, FLT_MAX);
  ADD_ATTR(node, FloatAttribute, "noise_ratio", 0.2f, 0.f, 1.f);
  ADD_ATTR(node, SeedAttribute, "seed");

  // attribute(s) order
  node.set_attr_ordered_key({"slope", "noise_ratio", "seed"});
}

void compute_fill_talus_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    float talus = GET(node, "slope", FloatAttribute) / (float)p_out->shape.x;

    // copy the input heightmap
    *p_out = *p_in;

    hmap::transform(*p_out,
                    [&node, &talus](hmap::Array &x)
                    {
                      hmap::fill_talus(x,
                                       talus,
                                       GET(node, "seed", SeedAttribute),
                                       GET(node, "noise_ratio", FloatAttribute));
                    });

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
