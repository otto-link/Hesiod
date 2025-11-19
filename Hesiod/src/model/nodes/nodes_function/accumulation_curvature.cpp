/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/curvature.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_accumulation_curvature_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "radius", 0.02f, 0.f, 0.2f);
  ADD_ATTR(node, BoolAttribute, "clamp_max", false);
  ADD_ATTR(node, FloatAttribute, "vc_max", 0.05f, 0.f, 0.2f);

  // attribute(s) order
  node.set_attr_ordered_key({"radius", "clamp_max", "vc_max"});

  setup_post_process_heightmap_attributes(node);
}

void compute_accumulation_curvature_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    int ir = std::max(1, (int)(GET(node, "radius", FloatAttribute) * p_out->shape.x));
    int nx = p_out->shape.x; // for gradient scaling

    hmap::transform(
        {p_out, p_in},
        [&node, ir, nx](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          // nx^2 is gradient scaling...
          *pa_out = nx * nx * hmap::gpu::accumulation_curvature(*pa_in, ir);

          // truncate high values if requested
          if (GET(node, "clamp_max", BoolAttribute))
            hmap::clamp_max(*pa_out, GET(node, "vc_max", FloatAttribute));
        },
        node.get_config_ref()->hmap_transform_mode_gpu);

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(node, *p_out);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
