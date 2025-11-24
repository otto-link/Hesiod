/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/transform.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_warp_downslope_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("amount", "amount", 0.02f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("radius", "radius", 0.05f, 0.f, 0.2f);
  node.add_attr<BoolAttribute>("reverse", "reverse", true);
}

void compute_warp_downslope_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    int ir = (int)(node.get_attr<FloatAttribute>("radius") * p_out->shape.x);

    // --- work on a single array (as a temporary solution?)
    hmap::Array z_array = p_in->to_array();

    hmap::Array  mask_array;
    hmap::Array *p_mask_array = nullptr;

    if (p_mask)
    {
      mask_array = p_mask->to_array();
      p_mask_array = &mask_array;
    }

    hmap::warp_downslope(z_array,
                         p_mask_array,
                         node.get_attr<FloatAttribute>("amount"),
                         ir,
                         node.get_attr<BoolAttribute>("reverse"));

    p_out->from_array_interp_nearest(z_array);
  }
}

} // namespace hesiod
