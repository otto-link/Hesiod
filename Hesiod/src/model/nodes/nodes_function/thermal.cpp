/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_thermal_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "bedrock");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "deposition", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("talus_global", 1.f, 0.f, 4.f, "talus_global");
  p_node->add_attr<IntAttribute>("iterations", 10, 1, 200, "iterations");

  // attribute(s) order
  p_node->set_attr_ordered_key({"talus_global", "iterations"});
}

void compute_thermal_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_bedrock = p_node->get_value_ref<hmap::HeightMap>("bedrock");
    hmap::HeightMap *p_mask = p_node->get_value_ref<hmap::HeightMap>("mask");

    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");
    hmap::HeightMap *p_deposition_map = p_node->get_value_ref<hmap::HeightMap>(
        "deposition");

    // copy the input heightmap
    *p_out = *p_in;

    float talus = GET("talus_global", FloatAttribute) / (float)p_out->shape.x;

    hmap::transform(*p_out,
                    p_bedrock,
                    p_mask,
                    p_deposition_map,
                    [p_node, &talus](hmap::Array &h_out,
                                     hmap::Array *p_bedrock_array,
                                     hmap::Array *p_mask_array,
                                     hmap::Array *p_deposition_array)
                    {
                      hmap::thermal(h_out,
                                    p_mask_array,
                                    hmap::constant(h_out.shape, talus),
                                    GET("iterations", IntAttribute),
                                    p_bedrock_array,
                                    p_deposition_array);
                    });

    p_out->smooth_overlap_buffers();

    if (p_deposition_map)
    {
      p_deposition_map->smooth_overlap_buffers();
      p_deposition_map->remap();
    }
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
