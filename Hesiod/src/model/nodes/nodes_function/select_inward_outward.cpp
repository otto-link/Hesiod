/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/selector.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_select_inward_outward_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(Vec2FloatAttribute, "center");
  ADD_ATTR(BoolAttribute, "inverse", false);
  ADD_ATTR(BoolAttribute, "smoothing", false);
  ADD_ATTR(FloatAttribute, "smoothing_radius", 0.05f, 0.f, 0.2f);

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"center", "_SEPARATOR_", "inverse", "smoothing", "smoothing_radius"});
}

void compute_select_inward_outward_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    hmap::transform(*p_out,
                    *p_in,
                    [p_node](hmap::Array &out, hmap::Array &in, hmap::Vec4<float> bbox)
                    {
                      out = hmap::select_inward_outward_slope(
                          in,
                          GET("center", Vec2FloatAttribute),
                          bbox);
                    });

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(p_node,
                           *p_out,
                           GET("inverse", BoolAttribute),
                           GET("smoothing", BoolAttribute),
                           GET("smoothing_radius", FloatAttribute),
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           true, // force remap
                           {0.f, 1.f});
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
