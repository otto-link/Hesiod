/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_depression_filling_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "fill map", CONFIG);

  // attribute(s)
  ADD_ATTR(IntAttribute, "iterations", 1000, 1, INT_MAX);
  ADD_ATTR(FloatAttribute, "epsilon", 1e-4, 1e-5, 1e-1);
  ADD_ATTR(BoolAttribute, "remap fill map", true);

  // attribute(s) order
  p_node->set_attr_ordered_key({"iterations", "epsilon", "remap fill map"});
}

void compute_depression_filling_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_fill_map = p_node->get_value_ref<hmap::Heightmap>("fill map");

    // work on a single array (as a temporary solution?)
    hmap::Array z_array = p_in->to_array();

    float epsilon_normalized = GET("epsilon", FloatAttribute) / (float)p_in->shape.x;

    hmap::depression_filling(z_array,
                             GET("iterations", IntAttribute),
                             epsilon_normalized);
    p_out->from_array_interp(z_array);

    hmap::transform(*p_fill_map,
                    *p_in,
                    *p_out,
                    [p_node](hmap::Array &out, hmap::Array &in1, hmap::Array &in2)
                    { out = in2 - in1; });

    if (GET("remap fill map", BoolAttribute))
      p_fill_map->remap();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
