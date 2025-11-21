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

void setup_directional_blur_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "angle");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("radius", "radius", 0.05f, 0.f, 0.2f);

  node.add_attr<FloatAttribute>("angle", "angle", 0.f, -180.f, 180.f);

  node.add_attr<FloatAttribute>("intensity", "intensity", 1.f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("spread", "spread", 1.f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("stretch", "stretch", 1.f, 0.f, 4.f);

  // attribute(s) order
  node.set_attr_ordered_key({"radius", "angle", "intensity", "spread", "stretch"});

  setup_post_process_heightmap_attributes(node, true);
}

void compute_directional_blur_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_angle = node.get_value_ref<hmap::Heightmap>("angle");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    int ir = std::max(1, (int)(node.get_attr<FloatAttribute>("radius") * p_out->shape.x));

    hmap::transform(
        {p_out, p_in, p_angle},
        [&node, ir](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];
          hmap::Array *pa_angle = p_arrays[2];

          hmap::Array angle_deg(pa_in->shape, node.get_attr<FloatAttribute>("angle"));
          if (pa_angle)
            angle_deg += (*pa_angle) * 180.f / M_PI;

          *pa_out = *pa_in;

          hmap::directional_blur(*pa_out,
                                 ir,
                                 angle_deg,
                                 node.get_attr<FloatAttribute>("intensity"),
                                 node.get_attr<FloatAttribute>("stretch"),
                                 node.get_attr<FloatAttribute>("spread"));
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(node, *p_out, p_in);
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
