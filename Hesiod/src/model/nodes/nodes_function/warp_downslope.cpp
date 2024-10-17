/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/transform.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_warp_downslope_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("amount", 0.02f, 0.f, 1.f, "amount");
  p_node->add_attr<FloatAttribute>("radius", 0.05f, 0.f, 0.2f, "radius");
  p_node->add_attr<BoolAttribute>("reverse", true, "reverse");
}

void compute_warp_downslope_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_mask = p_node->get_value_ref<hmap::HeightMap>("mask");
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

    int ir = (int)(GET("radius", FloatAttribute) * p_out->shape.x);

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
                         GET("amount", FloatAttribute),
                         ir,
                         GET("reverse", BoolAttribute));

    p_out->from_array_interp_nearest(z_array);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
