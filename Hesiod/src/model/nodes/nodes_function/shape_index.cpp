/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/curvature.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_shape_index_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "radius", 0.01f, 0.f, 0.2f);
  ADD_ATTR(node, BoolAttribute, "inverse", false);
  ADD_ATTR(node, BoolAttribute, "smoothing", false);
  ADD_ATTR(node, FloatAttribute, "smoothing_radius", 0.05f, 0.f, 0.2f);
  ADD_ATTR(node, BoolAttribute, "GPU", HSD_DEFAULT_GPU_MODE);

  // attribute(s) order
  node.set_attr_ordered_key({"radius",
                             "_SEPARATOR_",
                             "inverse",
                             "smoothing",
                             "smoothing_radius",
                             "_SEPARATOR_",
                             "GPU"});
}

void compute_shape_index_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // zero radius accepted
    int ir = std::max(0, (int)(GET(node, "radius", FloatAttribute) * p_out->shape.x));

    if (GET(node, "GPU", BoolAttribute))
    {
      hmap::transform(
          {p_out, p_in},
          [&ir](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_in = p_arrays[1];

            *pa_out = hmap::gpu::shape_index(*pa_in, ir);
          },
          node.get_config_ref()->hmap_transform_mode_gpu);
    }
    else
    {
      hmap::transform(
          {p_out, p_in},
          [&ir](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_in = p_arrays[1];

            *pa_out = hmap::shape_index(*pa_in, ir);
          },
          node.get_config_ref()->hmap_transform_mode_cpu);
    }

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(node,
                           *p_out,
                           GET(node, "inverse", BoolAttribute),
                           GET(node, "smoothing", BoolAttribute),
                           GET(node, "smoothing_radius", FloatAttribute),
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           false, // remap
                           {0.f, 0.f});
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
