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

void setup_hydraulic_musgrave_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "moisture");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, IntAttribute, "iterations", 100, 1, INT_MAX);
  ADD_ATTR(node, FloatAttribute, "c_capacity", 2.f, 0.1f, 40.f);
  ADD_ATTR(node, FloatAttribute, "c_erosion", 0.01f, 0.001f, 0.1f);
  ADD_ATTR(node, FloatAttribute, "c_deposition", 0.01f, 0.001f, 0.1f);
  ADD_ATTR(node, FloatAttribute, "water_level", 0.001f, 0.f, 0.1f);
  ADD_ATTR(node, FloatAttribute, "evap_rate", 0.001f, 0.f, 0.1f);

  // attribute(s) order
  node.set_attr_ordered_key({"iterations",
                             "c_capacity",
                             "c_erosion",
                             "c_deposition",
                             "water_level",
                             "evap_rate"});
}

void compute_hydraulic_musgrave_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_moisture_map = node.get_value_ref<hmap::Heightmap>("moisture");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    hmap::Heightmap moisture_map = p_moisture_map ? *p_moisture_map
                                                  : hmap::Heightmap(CONFIG, 1.f);

    hmap::transform(*p_out,
                    moisture_map,
                    [&node](hmap::Array &h_out, hmap::Array moisture_map_array)
                    {
                      hmap::hydraulic_musgrave(h_out,
                                               moisture_map_array,
                                               GET(node, "iterations", IntAttribute),
                                               GET(node, "c_capacity", FloatAttribute),
                                               GET(node, "c_erosion", FloatAttribute),
                                               GET(node, "c_deposition", FloatAttribute),
                                               GET(node, "water_level", FloatAttribute),
                                               GET(node, "evap_rate", FloatAttribute));
                    });

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
