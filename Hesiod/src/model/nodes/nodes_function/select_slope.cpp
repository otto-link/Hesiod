/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/gradient.hpp"
#include "highmap/morphology.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_select_slope_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("radius", 0.f, 0.f, 1.f, "radius");
  p_node->add_attr<BoolAttribute>("inverse", false, "inverse");
  p_node->add_attr<BoolAttribute>("smoothing", false, "smoothing");
  p_node->add_attr<FloatAttribute>("smoothing_radius",
                                   0.05f,
                                   0.f,
                                   0.2f,
                                   "smoothing_radius");
  p_node->add_attr<RangeAttribute>("saturate", "saturate", false);

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"radius", "_SEPARATOR_", "inverse", "smoothing", "smoothing_radius", "saturate"});
}

void compute_select_slope_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    int ir = (int)(GET("radius", FloatAttribute) * p_out->shape.x);

    if (ir > 0)
      hmap::transform(*p_out,
                      *p_in,
                      [p_node, ir](hmap::Array &out, hmap::Array &in)
                      { out = hmap::morphological_gradient(in, ir); });
    else
      hmap::transform(*p_out,
                      *p_in,
                      [p_node](hmap::Array &out, hmap::Array &in)
                      { out = hmap::gradient_norm(in); });

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(*p_out,
                           GET("inverse", BoolAttribute),
                           GET("smoothing", BoolAttribute),
                           GET("smoothing_radius", FloatAttribute),
                           GET_ATTR("saturate", RangeAttribute, is_active),
                           GET("saturate", RangeAttribute),
                           0.f,
                           true, // force remap
                           {0.f, 1.f});
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod