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

void setup_hydraulic_schott_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "flow_map", CONFIG);

  // attribute(s)
  ADD_ATTR(IntAttribute, "iterations", 100, 1, INT_MAX);
  ADD_ATTR(FloatAttribute, "c_erosion", 1.f, 0.f, 2.f);
  ADD_ATTR(FloatAttribute, "c_deposition", 0.4f, 0.f, 2.f);
  ADD_ATTR(FloatAttribute, "c_thermal", 0.1f, 0.f, 2.f);
  ADD_ATTR(FloatAttribute, "talus_global", 1.5f, 0.f, 16.f);
  ADD_ATTR(BoolAttribute, "scale_talus_with_elevation", false);
  ADD_ATTR(FloatAttribute, "flow_acc_exponent", 0.8f, 0.01f, 1.f);
  ADD_ATTR(FloatAttribute, "flow_acc_exponent_depo", 0.8f, 0.01f, 1.f);
  ADD_ATTR(FloatAttribute, "flow_routing_exponent", 1.3f, 0.01f, 2.f);
  ADD_ATTR(FloatAttribute, "thermal_weight", 0.f, 0.f, 5.f);
  ADD_ATTR(FloatAttribute, "deposition_weight", 1.f, 0.f, 5.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"iterations",
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

void compute_hydraulic_schott_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // AppContext &ctx = HSD_CTX;

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_flow_map = p_node->get_value_ref<hmap::Heightmap>("flow_map");

    // copy the input heightmap
    *p_out = *p_in;

    // talus
    float           talus = GET("talus_global", FloatAttribute) / (float)p_out->shape.x;
    hmap::Heightmap talus_map = hmap::Heightmap(CONFIG, talus);

    if (GET("scale_talus_with_elevation", BoolAttribute))
    {
      talus_map = *p_in;
      talus_map.remap(talus / 100.f, talus);
    }

    // use a flat flow map as input
    hmap::Heightmap flow_map = hmap::Heightmap(CONFIG, 1.f);

    hmap::transform(
        {p_out, &talus_map, p_mask, &flow_map},
        [p_node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_talus = p_arrays[1];
          hmap::Array *pa_mask = p_arrays[2];
          hmap::Array *pa_flow_map = p_arrays[3];

          hmap::gpu::hydraulic_schott(*pa_out,
                                      GET("iterations", IntAttribute),
                                      *pa_talus,
                                      pa_mask,
                                      GET("c_erosion", FloatAttribute),
                                      GET("c_thermal", FloatAttribute),
                                      GET("c_deposition", FloatAttribute),
                                      GET("flow_acc_exponent", FloatAttribute),
                                      GET("flow_acc_exponent_depo", FloatAttribute),
                                      GET("flow_routing_exponent", FloatAttribute),
                                      GET("thermal_weight", FloatAttribute),
                                      GET("deposition_weight", FloatAttribute),
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

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
