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

void setup_hydraulic_vpipes_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "bedrock");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "moisture");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "erosion", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG);

  // attribute(s)
  ADD_ATTR(IntAttribute, "iterations", 50, 1, INT_MAX);
  ADD_ATTR(FloatAttribute, "water_height", 0.01f, 0.01f, 0.5f);
  ADD_ATTR(FloatAttribute, "c_capacity", 0.1f, 0.01f, 0.5f);
  ADD_ATTR(FloatAttribute, "c_erosion", 0.01f, 0.f, 0.5f);
  ADD_ATTR(FloatAttribute, "c_deposition", 0.01f, 0.f, 0.5f);
  ADD_ATTR(FloatAttribute, "rain_rate", 0.f, 0.f, 0.1f);
  ADD_ATTR(FloatAttribute, "evap_rate", 0.01f, 0.01f, 0.1f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"iterations",
                                "water_height",
                                "c_capacity",
                                "c_erosion",
                                "c_deposition",
                                "rain_rate",
                                "evap_rate"});
}

void compute_hydraulic_vpipes_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_bedrock = p_node->get_value_ref<hmap::Heightmap>("bedrock");
    hmap::Heightmap *p_moisture_map = p_node->get_value_ref<hmap::Heightmap>("moisture");
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");

    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_erosion_map = p_node->get_value_ref<hmap::Heightmap>("erosion");
    hmap::Heightmap *p_deposition_map = p_node->get_value_ref<hmap::Heightmap>(
        "deposition");

    // copy the input heightmap
    *p_out = *p_in;

    hmap::transform(*p_out,
                    p_bedrock,
                    p_moisture_map,
                    p_mask,
                    p_erosion_map,
                    p_deposition_map,
                    [p_node](hmap::Array &h_out,
                             hmap::Array *p_bedrock_array,
                             hmap::Array *p_moisture_map_array,
                             hmap::Array *p_mask_array,
                             hmap::Array *p_erosion_map_array,
                             hmap::Array *p_deposition_map_array)
                    {
                      hydraulic_vpipes(h_out,
                                       p_mask_array,
                                       GET("iterations", IntAttribute),
                                       p_bedrock_array,
                                       p_moisture_map_array,
                                       p_erosion_map_array,
                                       p_deposition_map_array,
                                       GET("water_height", FloatAttribute),
                                       GET("c_capacity", FloatAttribute),
                                       GET("c_erosion", FloatAttribute),
                                       GET("c_deposition", FloatAttribute),
                                       GET("rain_rate", FloatAttribute),
                                       GET("evap_rate", FloatAttribute));
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

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
