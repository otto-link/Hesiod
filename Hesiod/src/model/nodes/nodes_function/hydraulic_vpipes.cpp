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
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "input");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "bedrock");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "moisture");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "mask");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "output", CONFIG(node));
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "erosion", CONFIG(node));
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "deposition", CONFIG(node));

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
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // hmap::VirtualArray *p_in = node.get_value_ref<hmap::VirtualArray>("input");

  // if (p_in)
  // {
  //   hmap::VirtualArray *p_bedrock = node.get_value_ref<hmap::VirtualArray>("bedrock");
  //   hmap::VirtualArray *p_moisture_map = node.get_value_ref<hmap::VirtualArray>(
  //       "moisture");
  //   hmap::VirtualArray *p_mask = node.get_value_ref<hmap::VirtualArray>("mask");

  //   hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("output");
  //   hmap::VirtualArray *p_erosion_map =
  //   node.get_value_ref<hmap::VirtualArray>("erosion"); hmap::VirtualArray
  //   *p_deposition_map = node.get_value_ref<hmap::VirtualArray>(
  //       "deposition");

  //   hmap::for_each_tile({p_out,
  // 	p_in,
  // 	  p_bedrock,
  //                       p_moisture_map,
  //                       p_mask,
  //                       p_erosion_map,
  //                       p_deposition_map},
  //       [&node](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &region)
  //       {
  //         hmap::Array *pa_out = p_arrays[0];
  //         hmap::Array *pa_in = p_arrays[1];
  //         hmap::Array *pa_bedrock = p_arrays[2];
  // 	  hmap::Array *pa_moisture_map = p_arrays[2];

  // 	  hmap::hydraulic_vpipes(*pa_out,
  //                                          pa_mask,
  //                                          node.get_attr<IntAttribute>("iterations"),
  //                                          pa_bedrock,
  //                                          pa_moisture_map,
  //                                          pa_erosion_map,
  //                                          pa_deposition_map,
  //                                          node.get_attr<FloatAttribute>("water_height"),
  //                                          node.get_attr<FloatAttribute>("c_capacity"),
  //                                          node.get_attr<FloatAttribute>("c_erosion"),
  //                                          node.get_attr<FloatAttribute>("c_deposition"),
  //                                          node.get_attr<FloatAttribute>("rain_rate"),
  //                                          node.get_attr<FloatAttribute>("evap_rate"));
  //                       },
  //       node.cfg().cm_cpu);

  //   p_out->smooth_overlap_buffers();

  //   if (p_erosion_map)
  //   {
  //     p_erosion_map->smooth_overlap_buffers();
  //     p_erosion_map->remap(0.f, 1.f, node.cfg().cm_cpu);
  //   }

  //   if (p_deposition_map)
  //   {
  //     p_deposition_map->smooth_overlap_buffers();
  //     p_deposition_map->remap(0.f, 1.f, node.cfg().cm_cpu);
  //   }
  // }
}

} // namespace hesiod
