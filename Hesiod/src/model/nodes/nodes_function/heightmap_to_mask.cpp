/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_heightmap_to_mask_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "mask", CONFIG);

  // attribute(s)
  p_node->add_attr<BoolAttribute>("inverse", false, "inverse");
  p_node->add_attr<BoolAttribute>("smoothing", false, "smoothing");
  p_node->add_attr<FloatAttribute>("smoothing_radius",
                                   0.05f,
                                   0.f,
                                   0.2f,
                                   "smoothing_radius");
  p_node->add_attr<RangeAttribute>("saturate_range", "saturate_range");
  p_node->add_attr<FloatAttribute>("saturate_k", 0.01f, 0.01f, 1.f, "saturate_k");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"inverse", "smoothing", "smoothing_radius", "saturate_range", "saturate_k"});
}

void compute_heightmap_to_mask_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_mask = p_node->get_value_ref<hmap::HeightMap>("mask");

    // copy the input heightmap
    *p_mask = *p_in;

    // clamp to [0, 1]
    hmap::transform(*p_mask, [](hmap::Array &x) { hmap::clamp(x, 0.f, 1.f); });

    // post-process
    post_process_heightmap(*p_mask,
                           GET("inverse", BoolAttribute),
                           GET("smoothing", BoolAttribute),
                           GET("smoothing_radius", FloatAttribute),
                           GET_ATTR("saturate_range", RangeAttribute, is_active),
                           GET("saturate_range", RangeAttribute),
                           GET("saturate_k", FloatAttribute),
                           false, // remap
                           {0.f, 0.f});
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
