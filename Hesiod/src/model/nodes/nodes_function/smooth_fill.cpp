/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/filters.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_smooth_fill_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "mask");

  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "deposition", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("radius", 0.05f, 0.001f, 0.2f, "radius");
  p_node->add_attr<FloatAttribute>("k", 0.01f, 0.01f, 1.f, "k");
  p_node->add_attr<BoolAttribute>("normalized_map", true, "normalized_map");

  // attribute(s) order
  p_node->set_attr_ordered_key({"radius", "k", "normalized_map"});
}

void compute_smooth_fill_node(BaseNode *p_node)
{
  LOG->trace("computing node {}", p_node->get_label());

  Q_EMIT p_node->compute_started(p_node->get_id());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_mask = p_node->get_value_ref<hmap::HeightMap>("mask");
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");
    hmap::HeightMap *p_deposition_map = p_node->get_value_ref<hmap::HeightMap>(
        "deposition");

    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));

    hmap::transform(
        *p_out,
        p_mask,
        p_deposition_map,
        [p_node, &ir](hmap::Array &x, hmap::Array *p_mask, hmap::Array *p_deposition)
        { hmap::smooth_fill(x, ir, p_mask, GET("k", FloatAttribute), p_deposition); });

    p_out->smooth_overlap_buffers();
    p_deposition_map->smooth_overlap_buffers();

    if (GET("normalized_map", BoolAttribute))
      p_deposition_map->remap();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
