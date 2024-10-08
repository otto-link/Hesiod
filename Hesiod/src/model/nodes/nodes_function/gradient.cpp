/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/gradient.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_gradient_node(BaseNode *p_node)
{
  LOG->trace("setup_gradient_node");

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "dx", CONFIG);
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "dy", CONFIG);

  // attribute(s)
  p_node->add_attr<RangeAttribute>("remap", "Remap range");
}

void compute_gradient_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_dx = p_node->get_value_ref<hmap::HeightMap>("dx");
    hmap::HeightMap *p_dy = p_node->get_value_ref<hmap::HeightMap>("dy");

    hmap::transform(*p_dx,
                    *p_in,
                    [](hmap::Array &out, hmap::Array &in) { hmap::gradient_x(in, out); });

    hmap::transform(*p_dy,
                    *p_in,
                    [](hmap::Array &out, hmap::Array &in) { hmap::gradient_y(in, out); });

    p_dx->smooth_overlap_buffers();
    p_dy->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(*p_dx,
                           false, // inverse
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_ATTR("remap", RangeAttribute, is_active),
                           GET("remap", RangeAttribute));

    post_process_heightmap(*p_dy,
                           false, // inverse
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_ATTR("remap", RangeAttribute, is_active),
                           GET("remap", RangeAttribute));
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod