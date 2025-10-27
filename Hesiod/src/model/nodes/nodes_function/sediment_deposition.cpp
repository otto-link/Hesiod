/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_sediment_deposition_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "talus_global", 0.2f, 0.f, FLT_MAX);
  ADD_ATTR(FloatAttribute, "max_deposition", 0.001f, 0.f, 0.1f);
  ADD_ATTR(IntAttribute, "iterations", 1, 1, 20);
  ADD_ATTR(IntAttribute, "thermal_subiterations", 50, 1, INT_MAX);
  ADD_ATTR(BoolAttribute, "scale_talus_with_elevation", true);

  // attribute(s) order
  p_node->set_attr_ordered_key({"talus_global",
                                "max_deposition",
                                "iterations",
                                "thermal_subiterations",
                                "scale_talus_with_elevation"});
}

void compute_sediment_deposition_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // AppContext &ctx = HSD_CTX;

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_deposition_map = p_node->get_value_ref<hmap::Heightmap>(
        "deposition");

    // copy the input heightmap
    *p_out = *p_in;

    float talus = GET("talus_global", FloatAttribute) / (float)p_out->shape.x;

    hmap::Heightmap talus_map = hmap::Heightmap(CONFIG, talus);

    if (GET("scale_talus_with_elevation", BoolAttribute))
    {
      talus_map = *p_in;
      talus_map.remap(talus / 100.f, talus);
    }

    hmap::transform(*p_out,
                    p_mask,
                    &talus_map,
                    p_deposition_map,
                    [p_node, &talus](hmap::Array &h_out,
                                     hmap::Array *p_mask_array,
                                     hmap::Array *p_talus_array,
                                     hmap::Array *p_deposition_array)
                    {
                      hmap::sediment_deposition(
                          h_out,
                          p_mask_array,
                          *p_talus_array,
                          p_deposition_array,
                          GET("max_deposition", FloatAttribute),
                          GET("iterations", IntAttribute),
                          GET("thermal_subiterations", IntAttribute));
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
