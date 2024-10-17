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

void setup_warp_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);
}

void compute_warp_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_dx = p_node->get_value_ref<hmap::HeightMap>("dx");
    hmap::HeightMap *p_dy = p_node->get_value_ref<hmap::HeightMap>("dy");
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

    // --- work on a single array (as a temporary solution?)
    hmap::Array z_array = p_in->to_array();

    hmap::Array  dx_array, dy_array;
    hmap::Array *p_dx_array = nullptr, *p_dy_array = nullptr;

    if (p_dx)
    {
      dx_array = p_dx->to_array();
      p_dx_array = &dx_array;
    }

    if (p_dy)
    {
      dy_array = p_dy->to_array();
      p_dy_array = &dy_array;
    }

    hmap::warp(z_array, p_dx_array, p_dy_array);

    p_out->from_array_interp_nearest(z_array);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
