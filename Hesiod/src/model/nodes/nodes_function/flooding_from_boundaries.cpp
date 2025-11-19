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
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_depth", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("elevation", "elevation", 0.2f, -1.f, 2.f);

  node.add_attr<BoolAttribute>("from_east", "from_east", true);

  node.add_attr<BoolAttribute>("from_west", "from_west", true);

  node.add_attr<BoolAttribute>("from_north", "from_north", true);

  node.add_attr<BoolAttribute>("from_south", "from_south", true);

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
              node.get_attr<FloatAttribute>("elevation"),
              node.get_attr<BoolAttribute>("from_east"),
              node.get_attr<BoolAttribute>("from_west"),
              node.get_attr<BoolAttribute>("from_north"),
              node.get_attr<BoolAttribute>("from_south"));
        },
        hmap::TransformMode::SINGLE_ARRAY); // forced, not tileable
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
