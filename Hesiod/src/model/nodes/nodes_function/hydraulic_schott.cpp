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
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "flow_map", CONFIG(node));

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
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_flow_map = node.get_value_ref<hmap::Heightmap>("flow_map");

    // copy the input heightmap
    *p_out = *p_in;

    // talus
    float talus = node.get_attr<FloatAttribute>("talus_global") / (float)p_out->shape.x;
    hmap::Heightmap talus_map = hmap::Heightmap(CONFIG(node), talus);

    if (node.get_attr<BoolAttribute>("scale_talus_with_elevation"))
    {
      talus_map = *p_in;
      talus_map.remap(talus / 100.f, talus);
    }

    // use a flat flow map as input
    hmap::Heightmap flow_map = hmap::Heightmap(CONFIG(node), 1.f);

    hmap::transform(
        {p_out, &talus_map, p_mask, &flow_map},
        [&node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_talus = p_arrays[1];
          hmap::Array *pa_mask = p_arrays[2];
          hmap::Array *pa_flow_map = p_arrays[3];

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
        hmap::TransformMode::SINGLE_ARRAY);

    p_out->smooth_overlap_buffers();

    if (p_flow_map)
    {
      *p_flow_map = flow_map;
      p_flow_map->smooth_overlap_buffers();
    }
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
