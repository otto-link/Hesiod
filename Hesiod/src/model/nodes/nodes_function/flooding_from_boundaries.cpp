/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/hydrology.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_flooding_from_boundaries_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_depth", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "elevation", 0.2f, -1.f, 2.f);
  ADD_ATTR(node, BoolAttribute, "from_east", true);
  ADD_ATTR(node, BoolAttribute, "from_west", true);
  ADD_ATTR(node, BoolAttribute, "from_north", true);
  ADD_ATTR(node, BoolAttribute, "from_south", true);

  // attribute(s) order
  node.set_attr_ordered_key(
      {"elevation", "from_east", "from_west", "from_north", "from_south"});
}

void compute_flooding_from_boundaries_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("water_depth");

    hmap::transform(
        {p_out, p_in},
        [&node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = hmap::flooding_from_boundaries(
              *pa_in,
              GET(node, "elevation", FloatAttribute),
              GET(node, "from_east", BoolAttribute),
              GET(node, "from_west", BoolAttribute),
              GET(node, "from_north", BoolAttribute),
              GET(node, "from_south", BoolAttribute));
        },
        hmap::TransformMode::SINGLE_ARRAY); // forced, not tileable
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
