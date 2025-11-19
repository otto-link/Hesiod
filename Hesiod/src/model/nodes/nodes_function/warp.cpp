/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/transform.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_warp_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "scaling.x", 1.f, -2.f, 2.f);
  ADD_ATTR(node, FloatAttribute, "scaling.y", 1.f, -2.f, 2.f);

  // attribute(s) order
  node.set_attr_ordered_key({"scaling.x", "scaling.y"});

  setup_post_process_heightmap_attributes(node, true);
}

void compute_warp_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
    hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    *p_out = *p_in;

    float sx = GET(node, "scaling.x", FloatAttribute);
    float sy = GET(node, "scaling.y", FloatAttribute);

    hmap::transform(
        {p_out, p_dx, p_dy},
        [sx, sy](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array dx = p_arrays[1] ? sx * (*p_arrays[1]) : hmap::Array(pa_out->shape);
          hmap::Array dy = p_arrays[2] ? sy * (*p_arrays[2]) : hmap::Array(pa_out->shape);

          hmap::gpu::warp(*pa_out, &dx, &dy);
        },
        node.get_config_ref()->hmap_transform_mode_gpu);

    // post-process
    post_process_heightmap(node, *p_out, p_in);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
