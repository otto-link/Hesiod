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

void setup_hydraulic_vpipes_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "bedrock");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "moisture");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "erosion", CONFIG);
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG);

  // attribute(s)
  ADD_ATTR(node, IntAttribute, "iterations", 50, 1, INT_MAX);
  ADD_ATTR(node, FloatAttribute, "water_height", 0.01f, 0.01f, 0.5f);
  ADD_ATTR(node, FloatAttribute, "c_capacity", 0.1f, 0.01f, 0.5f);
  ADD_ATTR(node, FloatAttribute, "c_erosion", 0.01f, 0.f, 0.5f);
  ADD_ATTR(node, FloatAttribute, "c_deposition", 0.01f, 0.f, 0.5f);
  ADD_ATTR(node, FloatAttribute, "rain_rate", 0.f, 0.f, 0.1f);
  ADD_ATTR(node, FloatAttribute, "evap_rate", 0.01f, 0.01f, 0.1f);

  // attribute(s) order
  node.set_attr_ordered_key({"iterations",
                             "water_height",
                             "c_capacity",
                             "c_erosion",
                             "c_deposition",
                             "rain_rate",
                             "evap_rate"});
}

void compute_hydraulic_vpipes_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_bedrock = node.get_value_ref<hmap::Heightmap>("bedrock");
    hmap::Heightmap *p_moisture_map = node.get_value_ref<hmap::Heightmap>("moisture");
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");

    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_erosion_map = node.get_value_ref<hmap::Heightmap>("erosion");
    hmap::Heightmap *p_deposition_map = node.get_value_ref<hmap::Heightmap>("deposition");

    // copy the input heightmap
    *p_out = *p_in;

    hmap::transform(*p_out,
                    p_bedrock,
                    p_moisture_map,
                    p_mask,
                    p_erosion_map,
                    p_deposition_map,
                    [&node](hmap::Array &h_out,
                            hmap::Array *p_bedrock_array,
                            hmap::Array *p_moisture_map_array,
                            hmap::Array *p_mask_array,
                            hmap::Array *p_erosion_map_array,
                            hmap::Array *p_deposition_map_array)
                    {
                      hydraulic_vpipes(h_out,
                                       p_mask_array,
                                       GET(node, "iterations", IntAttribute),
                                       p_bedrock_array,
                                       p_moisture_map_array,
                                       p_erosion_map_array,
                                       p_deposition_map_array,
                                       GET(node, "water_height", FloatAttribute),
                                       GET(node, "c_capacity", FloatAttribute),
                                       GET(node, "c_erosion", FloatAttribute),
                                       GET(node, "c_deposition", FloatAttribute),
                                       GET(node, "rain_rate", FloatAttribute),
                                       GET(node, "evap_rate", FloatAttribute));
                    });

    p_out->smooth_overlap_buffers();

    if (p_erosion_map)
    {
      p_erosion_map->smooth_overlap_buffers();
      p_erosion_map->remap();
    }

    if (p_deposition_map)
    {
      p_deposition_map->smooth_overlap_buffers();
      p_deposition_map->remap();
    }
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
