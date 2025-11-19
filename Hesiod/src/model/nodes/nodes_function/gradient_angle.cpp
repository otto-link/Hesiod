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

void setup_gradient_angle_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "smoothing_radius", 0.f, 0.f, 0.2f);
  ADD_ATTR(node,
           RangeAttribute,
           "remap",
           std::vector<float>({-1.f, 1.f}),
           -1.f,
           1.f,
           false);

  // attribute(s) order
  node.set_attr_ordered_key({"_TEXT_Post-processing", "smoothing_radius", "remap"});
}

void compute_gradient_angle_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    int ir = (int)(GET(node, "smoothing_radius", FloatAttribute) * p_out->shape.x);

    hmap::transform(
        {p_out, p_in},
        [ir](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          if (ir > 0)
            *pa_out = hmap::gradient_angle_circular_smoothing(*pa_in, ir);
          else
            *pa_out = hmap::gradient_angle(*pa_in);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(node,
                           *p_out,
                           false, // inverse
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_MEMBER(node, "remap", RangeAttribute, is_active),
                           GET(node, "remap", RangeAttribute));
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
