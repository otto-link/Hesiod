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

void setup_flooding_from_boundaries_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_depth", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "elevation", 0.2f, -1.f, 2.f);
  ADD_ATTR(BoolAttribute, "from_east", true);
  ADD_ATTR(BoolAttribute, "from_west", true);
  ADD_ATTR(BoolAttribute, "from_north", true);
  ADD_ATTR(BoolAttribute, "from_south", true);

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"elevation", "from_east", "from_west", "from_north", "from_south"});
}

void compute_flooding_from_boundaries_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("water_depth");

    hmap::transform(
        {p_out, p_in},
        [p_node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = hmap::flooding_from_boundaries(*pa_in,
                                                   GET("elevation", FloatAttribute),
                                                   GET("from_east", BoolAttribute),
                                                   GET("from_west", BoolAttribute),
                                                   GET("from_north", BoolAttribute),
                                                   GET("from_south", BoolAttribute));
        },
        hmap::TransformMode::SINGLE_ARRAY); // forced, not tileable
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
