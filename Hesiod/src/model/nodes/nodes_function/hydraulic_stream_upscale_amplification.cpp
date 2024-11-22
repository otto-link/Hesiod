/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/erosion.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

void setup_hydraulic_stream_upscale_amplification_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("c_erosion", 0.01f, 0.001f, 0.1f, "c_erosion");
  p_node->add_attr<FloatAttribute>("talus_ref", 0.1f, 0.01f, 10.f, "talus_ref");
  p_node->add_attr<FloatAttribute>("radius", 0.f, 0.f, 0.05f, "radius");
  p_node->add_attr<FloatAttribute>("clipping_ratio", 10.f, 0.1f, 100.f, "clipping_ratio");

  p_node->add_attr<IntAttribute>("upscaling_levels", 1, 0, 4, "upscaling_levels");
  p_node->add_attr<FloatAttribute>("persistence", 0.5f, 0.f, 1.f, "persistence");

  // attribute(s) order
  p_node->set_attr_ordered_key({"c_erosion",
                                "talus_ref",
                                "radius",
                                "clipping_ratio",
                                "_SEPARATOR_",
                                "upscaling_levels",
                                "persistence"});
}

void compute_hydraulic_stream_upscale_amplification_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    int ir = (int)(GET("radius", FloatAttribute) * p_out->shape.x);

    hmap::transform(*p_out,
                    p_mask,
                    [p_node, &ir](hmap::Array &h_out, hmap::Array *p_mask_array)
                    {
                      hmap::hydraulic_stream_upscale_amplification(
                          h_out,
                          p_mask_array,
                          GET("c_erosion", FloatAttribute),
                          GET("talus_ref", FloatAttribute),
                          GET("upscaling_levels", IntAttribute),
                          GET("persistence", FloatAttribute),
                          ir,
                          GET("clipping_ratio", FloatAttribute));
                    });

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
