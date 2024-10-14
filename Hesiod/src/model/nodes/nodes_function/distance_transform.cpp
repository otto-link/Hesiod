/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/morphology.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_distance_transform_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<BoolAttribute>("reverse", false, "reverse");
  p_node->add_attr<BoolAttribute>("inverse", false, "inverse");
  p_node->add_attr<BoolAttribute>("remap", true, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key({"reverse", "_SEPARATOR_", "inverse", "remap"});
}

void compute_distance_transform_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

    // not distributed, work on a single array
    hmap::Array z_array = p_in->to_array();

    if (GET("reverse", BoolAttribute))
    {
      make_binary(z_array);
      z_array = 1.f - z_array;
    }

    z_array = hmap::distance_transform(z_array);

    p_out->from_array_interp(z_array);

    // post-process
    post_process_heightmap(*p_out,
                           GET("inverse", BoolAttribute),
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET("remap", BoolAttribute),
                           {0.f, 1.f});
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod