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

void setup_hydraulic_schott_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "softness");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "flow_map", CONFIG);

  // attribute(s)
  p_node->add_attr<IntAttribute>("iterations", 60, 1, 200, "iterations");
  p_node->add_attr<FloatAttribute>("deposition_iterations_ratio",
                                   0.5f,
                                   0.f,
                                   8.f,
                                   "deposition_iterations_ratio");
  p_node->add_attr<FloatAttribute>("c_erosion", 0.4f, 0.01f, 1.f, "c_erosion");
  p_node->add_attr<FloatAttribute>("c_deposition", 0.5f, 0.01f, 1.f, "c_deposition");

  // attribute(s) order
  p_node->set_attr_ordered_key({"iterations",
                                "deposition_iterations_ratio",
                                "_SEPARATOR_",
                                "c_erosion",
                                "c_deposition"});
}

void compute_hydraulic_schott_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_softness = p_node->get_value_ref<hmap::Heightmap>("softness");

    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_flow_map = p_node->get_value_ref<hmap::Heightmap>("flow_map");
        
    // copy the input heightmap
    *p_out = *p_in;

    // use a flat flow map as input
    hmap::Heightmap flow_map = hmap::Heightmap(CONFIG, 1.f);
    
    hmap::transform(*p_out,
                    p_mask,
                    p_softness,
                    &flow_map,
                    [p_node](hmap::Array &h_out,
                             hmap::Array *p_mask_array,
                             hmap::Array *p_softness_array,
                             hmap::Array *p_flow_map_array)
                    {
                      hmap::hydraulic_schott(
                          h_out,
                          p_mask_array,
                          GET("iterations", IntAttribute),
                          GET("deposition_iterations_ratio", FloatAttribute),
                          GET("c_erosion", FloatAttribute),
                          GET("c_deposition", FloatAttribute),
                          p_softness_array,
                          p_flow_map_array);
                    });

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
