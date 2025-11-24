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
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "erosion", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG(node));

  // attribute(s)
  node.add_attr<IntAttribute>("iterations", "iterations", 50, 1, INT_MAX);
  node.add_attr<FloatAttribute>("water_height", "water_height", 0.01f, 0.01f, 0.5f);
  node.add_attr<FloatAttribute>("c_capacity", "c_capacity", 0.1f, 0.01f, 0.5f);
  node.add_attr<FloatAttribute>("c_erosion", "c_erosion", 0.01f, 0.f, 0.5f);
  node.add_attr<FloatAttribute>("c_deposition", "c_deposition", 0.01f, 0.f, 0.5f);
  node.add_attr<FloatAttribute>("rain_rate", "rain_rate", 0.f, 0.f, 0.1f);
  node.add_attr<FloatAttribute>("evap_rate", "evap_rate", 0.01f, 0.01f, 0.1f);

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
  if (node.compute_started)
    node.compute_finished(node.get_id());

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
                                       node.get_attr<IntAttribute>("iterations"),
                                       p_bedrock_array,
                                       p_moisture_map_array,
                                       p_erosion_map_array,
                                       p_deposition_map_array,
                                       node.get_attr<FloatAttribute>("water_height"),
                                       node.get_attr<FloatAttribute>("c_capacity"),
                                       node.get_attr<FloatAttribute>("c_erosion"),
                                       node.get_attr<FloatAttribute>("c_deposition"),
                                       node.get_attr<FloatAttribute>("rain_rate"),
                                       node.get_attr<FloatAttribute>("evap_rate"));
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

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
