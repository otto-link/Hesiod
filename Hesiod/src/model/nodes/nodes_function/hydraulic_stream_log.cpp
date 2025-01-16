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

void setup_hydraulic_stream_log_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "erosion", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "flow_map", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("c_erosion", 0.05f, 0.01f, 0.1f, "c_erosion");
  p_node->add_attr<FloatAttribute>("saturation_ratio",
                                   1.f,
                                   0.01f,
                                   1.f,
                                   "saturation_ratio");
  p_node->add_attr<FloatAttribute>("gamma", 1.f, 0.01f, 10.f, "gamma");
  p_node->add_attr<FloatAttribute>("talus_ref", 0.1f, 0.01f, 10.f, "talus_ref");
  p_node->add_attr<FloatAttribute>("radius", 0.f, 0.f, 0.05f, "radius");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"c_erosion", "gamma", "saturation_ratio", "talus_ref", "radius"});
}

void compute_hydraulic_stream_log_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_erosion_map = p_node->get_value_ref<hmap::Heightmap>("erosion");
    hmap::Heightmap *p_flow_map = p_node->get_value_ref<hmap::Heightmap>("flow_map");

    // copy the input heightmap
    *p_out = *p_in;

    int ir = (int)(GET("radius", FloatAttribute) * p_out->shape.x);

    hmap::transform(
        {p_out, p_mask, p_erosion_map, p_flow_map},
        [p_node,
         &ir](std::vector<hmap::Array *> p_arrays, hmap::Vec2<int>, hmap::Vec4<float>)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_mask = p_arrays[1];
          hmap::Array *pa_erosion_map = p_arrays[2];
          hmap::Array *pa_flow_map = p_arrays[3];

          hmap::hydraulic_stream_log(*pa_out,
                                     GET("c_erosion", FloatAttribute),
                                     GET("talus_ref", FloatAttribute),
                                     pa_mask,
                                     GET("gamma", FloatAttribute),
                                     GET("saturation_ratio", FloatAttribute),
                                     nullptr,
                                     nullptr,
                                     pa_erosion_map,
                                     pa_flow_map,
                                     ir);
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);

    p_out->smooth_overlap_buffers();

    p_erosion_map->smooth_overlap_buffers();
    p_erosion_map->remap();

    p_flow_map->smooth_overlap_buffers();
    p_flow_map->remap();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
