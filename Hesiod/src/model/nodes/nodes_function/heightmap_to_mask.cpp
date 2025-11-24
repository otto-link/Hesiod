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
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "mask", CONFIG(node));

  // attribute(s)
  node.add_attr<BoolAttribute>("inverse", "inverse", false);
  node.add_attr<BoolAttribute>("smoothing", "smoothing", false);
  node.add_attr<FloatAttribute>("smoothing_radius", "smoothing_radius", 0.05f, 0.f, 0.2f);
  node.add_attr<RangeAttribute>("saturate_range", "saturate_range");
  node.add_attr<FloatAttribute>("saturate_k", "saturate_k", 0.01f, 0.01f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key(
      {"inverse", "smoothing", "smoothing_radius", "saturate_range", "saturate_k"});
}

void compute_heightmap_to_mask_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

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
    post_process_heightmap(
        node,
        *p_mask,
        node.get_attr<BoolAttribute>("inverse"),
        node.get_attr<BoolAttribute>("smoothing"),
        node.get_attr<FloatAttribute>("smoothing_radius"),
        node.get_attr_ref<RangeAttribute>("saturate_range")->get_is_active(),
        node.get_attr<RangeAttribute>("saturate_range"),
        node.get_attr<FloatAttribute>("saturate_k"),
        false, // remap
        {0.f, 0.f});
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
