/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_heightmap_to_mask_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "mask", CONFIG);

  // attribute(s)
  ADD_ATTR(node, BoolAttribute, "inverse", false);
  ADD_ATTR(node, BoolAttribute, "smoothing", false);
  ADD_ATTR(node, FloatAttribute, "smoothing_radius", 0.05f, 0.f, 0.2f);
  ADD_ATTR(node, RangeAttribute, "saturate_range");
  ADD_ATTR(node, FloatAttribute, "saturate_k", 0.01f, 0.01f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key(
      {"inverse", "smoothing", "smoothing_radius", "saturate_range", "saturate_k"});
}

void compute_heightmap_to_mask_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");

    // copy the input heightmap
    *p_mask = *p_in;

    // clamp to [0, 1]
    hmap::transform(*p_mask, [](hmap::Array &x) { hmap::clamp(x, 0.f, 1.f); });

    // post-process
    post_process_heightmap(node,
                           *p_mask,
                           GET(node, "inverse", BoolAttribute),
                           GET(node, "smoothing", BoolAttribute),
                           GET(node, "smoothing_radius", FloatAttribute),
                           GET_MEMBER(node, "saturate_range", RangeAttribute, is_active),
                           GET(node, "saturate_range", RangeAttribute),
                           GET(node, "saturate_k", FloatAttribute),
                           false, // remap
                           {0.f, 0.f});
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
