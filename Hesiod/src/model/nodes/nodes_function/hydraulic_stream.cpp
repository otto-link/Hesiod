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

void setup_hydraulic_stream_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "bedrock");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "moisture");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "erosion", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("c_erosion", 0.05f, 0.01f, 0.1f, "c_erosion");
  p_node->add_attr<FloatAttribute>("talus_ref", 0.1f, 0.01f, 10.f, "talus_ref");
  p_node->add_attr<FloatAttribute>("radius", 0.f, 0.f, 0.05f, "radius");
  p_node->add_attr<FloatAttribute>("clipping_ratio", 10.f, 0.1f, 100.f, "clipping_ratio");

  // attribute(s) order
  p_node->set_attr_ordered_key({"c_erosion", "talus_ref", "radius", "clipping_ratio"});
}

void compute_hydraulic_stream_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_bedrock = p_node->get_value_ref<hmap::Heightmap>("bedrock");
    hmap::Heightmap *p_moisture_map = p_node->get_value_ref<hmap::Heightmap>("moisture");
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");

    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_erosion_map = p_node->get_value_ref<hmap::Heightmap>("erosion");

    // copy the input heightmap
    *p_out = *p_in;

    int ir = (int)(GET("radius", FloatAttribute) * p_out->shape.x);

    hmap::transform(*p_out,
                    p_bedrock,
                    p_moisture_map,
                    p_mask,
                    p_erosion_map,
                    nullptr, // p_deposition_map,
                    [p_node, &ir](hmap::Array &h_out,
                                  hmap::Array *p_bedrock_array,
                                  hmap::Array *p_moisture_map_array,
                                  hmap::Array *p_mask_array,
                                  hmap::Array *p_erosion_map_array,
                                  hmap::Array * /*p_deposition_map_array*/)
                    {
                      hmap::hydraulic_stream(h_out,
                                             p_mask_array,
                                             GET("c_erosion", FloatAttribute),
                                             GET("talus_ref", FloatAttribute),
                                             p_bedrock_array,
                                             p_moisture_map_array,
                                             p_erosion_map_array,
                                             ir,
                                             GET("clipping_ratio", FloatAttribute));
                    });

    p_out->smooth_overlap_buffers();

    if (p_erosion_map)
      p_erosion_map->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
