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

void setup_shape_index_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "radius", 0.01f, 0.f, 0.2f);
  ADD_ATTR(BoolAttribute, "inverse", false);
  ADD_ATTR(BoolAttribute, "smoothing", false);
  ADD_ATTR(FloatAttribute, "smoothing_radius", 0.05f, 0.f, 0.2f);
  ADD_ATTR(BoolAttribute, "GPU", HSD_DEFAULT_GPU_MODE);

  // attribute(s) order
  p_node->set_attr_ordered_key({"radius",
                                "_SEPARATOR_",
                                "inverse",
                                "smoothing",
                                "smoothing_radius",
                                "_SEPARATOR_",
                                "GPU"});
}

void compute_shape_index_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // zero radius accepted
    int ir = std::max(0, (int)(GET("radius", FloatAttribute) * p_out->shape.x));

    if (GET("GPU", BoolAttribute))
    {
      hmap::transform(
          {p_out, p_in},
          [&ir](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_in = p_arrays[1];

            *pa_out = hmap::gpu::shape_index(*pa_in, ir);
          },
          p_node->get_config_ref()->hmap_transform_mode_gpu);
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
          p_node->get_config_ref()->hmap_transform_mode_cpu);
    }

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(p_node,
                           *p_out,
                           GET("inverse", BoolAttribute),
                           GET("smoothing", BoolAttribute),
                           GET("smoothing_radius", FloatAttribute),
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           false, // remap
                           {0.f, 0.f});
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
