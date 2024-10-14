/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_depression_filling_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "fill map", CONFIG);

  // attribute(s)
  p_node->add_attr<IntAttribute>("iterations", 1000, 1, 5000, "iterations");
  p_node->add_attr<FloatAttribute>("epsilon", 1e-4, 1e-5, 1e-2, "epsilon");
  p_node->add_attr<BoolAttribute>("remap fill map", true, "remap fill map");

  // attribute(s) order
  p_node->set_attr_ordered_key({"iterations", "epsilon", "remap fill map"});
}

void compute_depression_filling_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");
    hmap::HeightMap *p_fill_map = p_node->get_value_ref<hmap::HeightMap>("fill map");

    // work on a single array (as a temporary solution?)
    hmap::Array z_array = p_in->to_array();

    hmap::depression_filling(z_array,
                             GET("iterations", IntAttribute),
                             GET("epsilon", FloatAttribute));
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