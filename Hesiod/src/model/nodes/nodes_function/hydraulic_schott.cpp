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

void setup_hydraulic_schott_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "input");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "mask");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "output", CONFIG2(node));
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "flow_map", CONFIG2(node));

  // attribute(s)
  node.add_attr<IntAttribute>("iterations", "iterations", 100, 1, INT_MAX);
  node.add_attr<FloatAttribute>("c_erosion", "c_erosion", 1.f, 0.f, 2.f);
  node.add_attr<FloatAttribute>("c_deposition", "c_deposition", 0.4f, 0.f, 2.f);
  node.add_attr<FloatAttribute>("c_thermal", "c_thermal", 0.1f, 0.f, 2.f);
  node.add_attr<FloatAttribute>("talus_global", "talus_global", 1.5f, 0.f, 16.f);
  node.add_attr<BoolAttribute>("scale_talus_with_elevation",
                               "scale_talus_with_elevation",
                               false);
  node.add_attr<FloatAttribute>("flow_acc_exponent",
                                "flow_acc_exponent",
                                0.8f,
                                0.01f,
                                1.f);
  node.add_attr<FloatAttribute>("flow_acc_exponent_depo",
                                "flow_acc_exponent_depo",
                                0.8f,
                                0.01f,
                                1.f);
  node.add_attr<FloatAttribute>("flow_routing_exponent",
                                "flow_routing_exponent",
                                1.3f,
                                0.01f,
                                2.f);
  node.add_attr<FloatAttribute>("thermal_weight", "thermal_weight", 0.f, 0.f, 5.f);
  node.add_attr<FloatAttribute>("deposition_weight", "deposition_weight", 1.f, 0.f, 5.f);

  // attribute(s) order
  node.set_attr_ordered_key({"iterations",
                             "c_erosion",
                             "c_deposition",
                             "_SEPARATOR_",
                             "c_thermal",
                             "talus_global",
                             "scale_talus_with_elevation",
                             "_SEPARATOR_",
                             "flow_acc_exponent",
                             "flow_acc_exponent_depo",
                             "flow_routing_exponent",
                             "_SEPARATOR_",
                             "thermal_weight",
                             "deposition_weight"});
}

void compute_hydraulic_schott_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualArray *p_in = node.get_value_ref<hmap::VirtualArray>("input");

  if (p_in)
  {
    hmap::VirtualArray *p_mask = node.get_value_ref<hmap::VirtualArray>("mask");
    hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("output");
    hmap::VirtualArray *p_flow_map = node.get_value_ref<hmap::VirtualArray>("flow_map");

    // talus
    float talus = node.get_attr<FloatAttribute>("talus_global") / (float)p_out->shape.x;
    hmap::VirtualArray talus_map = hmap::VirtualArray(CONFIG2(node));
    talus_map.fill(talus, node.cfg().cm_cpu);

    if (node.get_attr<BoolAttribute>("scale_talus_with_elevation"))
    {
      talus_map.copy_from(*p_in, node.cfg().cm_cpu);
      talus_map.remap(talus / 100.f, talus, node.cfg().cm_cpu);
    }

    // use a flat flow map as input
    hmap::VirtualArray flow_map = hmap::VirtualArray(CONFIG2(node));

    hmap::for_each_tile(
        {p_out, p_in, &talus_map, p_mask, &flow_map},
        [&node](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];
          hmap::Array *pa_talus = p_arrays[2];
          hmap::Array *pa_mask = p_arrays[3];
          hmap::Array *pa_flow_map = p_arrays[4];

          *pa_out = *pa_in;
          *pa_flow_map = 1.f;

          hmap::gpu::hydraulic_schott(
              *pa_out,
              node.get_attr<IntAttribute>("iterations"),
              *pa_talus,
              pa_mask,
              node.get_attr<FloatAttribute>("c_erosion"),
              node.get_attr<FloatAttribute>("c_thermal"),
              node.get_attr<FloatAttribute>("c_deposition"),
              node.get_attr<FloatAttribute>("flow_acc_exponent"),
              node.get_attr<FloatAttribute>("flow_acc_exponent_depo"),
              node.get_attr<FloatAttribute>("flow_routing_exponent"),
              node.get_attr<FloatAttribute>("thermal_weight"),
              node.get_attr<FloatAttribute>("deposition_weight"),
              pa_flow_map);
        },
        node.cfg().cm_gpu);

    p_out->smooth_overlap_buffers();

    if (p_flow_map)
    {
      p_flow_map->copy_from(flow_map, node.cfg().cm_cpu);
      p_flow_map->smooth_overlap_buffers();
    }
  }
}

} // namespace hesiod
