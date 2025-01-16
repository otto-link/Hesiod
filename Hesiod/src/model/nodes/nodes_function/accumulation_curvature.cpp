/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/features.hpp"
#include "highmap/heightmap.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_accumulation_curvature_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("radius", 0.01f, 0.f, 0.2f, "radius");
  p_node->add_attr<BoolAttribute>("remap", true, "remap");
  p_node->add_attr<BoolAttribute>("inverse", false, "inverse");
  p_node->add_attr<BoolAttribute>("smoothing", false, "smoothing");
  p_node->add_attr<FloatAttribute>("smoothing_radius",
                                   0.05f,
                                   0.f,
                                   0.2f,
                                   "smoothing_radius");
  p_node->add_attr<BoolAttribute>("GPU", HSD_DEFAULT_GPU_MODE, "GPU");

  // attribute(s) order
  p_node->set_attr_ordered_key({"radius",
                                "_SEPARATOR_",
                                "remap",
                                "inverse",
                                "smoothing",
                                "smoothing_radius",
                                "_SEPARATOR_",
                                "GPU"});
}

void compute_accumulation_curvature_node(BaseNode *p_node)
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
          [&ir](std::vector<hmap::Array *> p_arrays, hmap::Vec2<int>, hmap::Vec4<float>)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_in = p_arrays[1];

            *pa_out = hmap::gpu::accumulation_curvature(*pa_in, ir);
          },
          p_node->get_config_ref()->hmap_transform_mode_gpu);
    }
    else
    {
      hmap::transform(
          {p_out, p_in},
          [&ir](std::vector<hmap::Array *> p_arrays, hmap::Vec2<int>, hmap::Vec4<float>)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_in = p_arrays[1];

            *pa_out = hmap::accumulation_curvature(*pa_in, ir);
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
                           GET("remap", BoolAttribute),
                           {0.f, 1.f});
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
