/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_directional_blur_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "angle");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "radius", 0.05f, 0.f, 0.2f);
  ADD_ATTR(FloatAttribute, "angle", 0.f, -180.f, 180.f);
  ADD_ATTR(FloatAttribute, "intensity", 1.f, 0.f, 1.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"radius", "angle", "intensity"});

  setup_post_process_heightmap_attributes(p_node, true);
}

void compute_directional_blur_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_angle = p_node->get_value_ref<hmap::Heightmap>("angle");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    int ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));

    hmap::transform(
        {p_out, p_in, p_angle},
        [p_node, ir](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];
          hmap::Array *pa_angle = p_arrays[2];

          hmap::Array angle_deg(pa_in->shape, GET("angle", FloatAttribute));
          if (pa_angle)
            angle_deg += (*pa_angle) * 180.f / M_PI;

          *pa_out = *pa_in;

          hmap::directional_blur(*pa_out,
                                 ir,
                                 angle_deg,
                                 GET("intensity", FloatAttribute));
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(p_node, *p_out, p_in);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
