/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/gradient.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_gradient_angle_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(BoolAttribute, "unwrap_angle", true);
  ADD_ATTR(RangeAttribute, "remap", std::vector<float>({-1.f, 1.f}), -1.f, 1.f, false);

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"_TEXT_Base paramaters", "unwrap_angle", "_TEXT_Post-processing", "remap"});
}

void compute_gradient_angle_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    hmap::transform(
        {p_out, p_in},
        [](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = hmap::gradient_angle(*pa_in);
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);

    p_out->smooth_overlap_buffers();

    // remove phase jumps if requested
    if (GET("unwrap_angle", BoolAttribute))
    {
      // on a single array, not tiled (so far)
      hmap::transform(
          {p_out},
          [](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            *pa_out = hmap::unwrap_phase(*pa_out);
          },
          hmap::TransformMode::SINGLE_ARRAY);
    }

    // post-process
    post_process_heightmap(p_node,
                           *p_out,
                           false, // inverse
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_MEMBER("remap", RangeAttribute, is_active),
                           GET("remap", RangeAttribute));
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
