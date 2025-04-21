/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/gradient.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_gradient_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "dx", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "dy", CONFIG);

  // attribute(s)
  ADD_ATTR(RangeAttribute, "remap");
}

void compute_gradient_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
    hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");

    hmap::transform(
        {p_dx, p_in},
        [](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_dx = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          hmap::gradient_x(*pa_in, *pa_dx);
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);

    hmap::transform(
        {p_dy, p_in},
        [](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_dy = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          hmap::gradient_y(*pa_in, *pa_dy);
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);

    p_dx->smooth_overlap_buffers();
    p_dy->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(p_node,
                           *p_dx,
                           false, // inverse
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_ATTR("remap", RangeAttribute, is_active),
                           GET("remap", RangeAttribute));

    post_process_heightmap(p_node,
                           *p_dy,
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
