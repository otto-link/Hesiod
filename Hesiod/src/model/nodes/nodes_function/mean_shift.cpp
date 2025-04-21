/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/kernels.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_mean_shift_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "radius", 0.05f, 0.01f, 0.2f);
  ADD_ATTR(FloatAttribute, "talus_global", 16.f, 0.f, FLT_MAX);
  ADD_ATTR(IntAttribute, "iterations", 4, 1, 10);
  ADD_ATTR(BoolAttribute, "talus_weighted", true);
  ADD_ATTR(BoolAttribute, "GPU", HSD_DEFAULT_GPU_MODE);

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"radius", "talus_global", "iterations", "talus_weighted", "_SEPARATOR_", "GPU"});
}

void compute_mean_shift_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    int   ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));
    float talus = GET("talus_global", FloatAttribute) / (float)p_out->shape.x;

    if (GET("GPU", BoolAttribute))
    {
      hmap::transform(
          {p_out, p_in},
          [p_node, ir, talus](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_in = p_arrays[1];

            *pa_out = hmap::gpu::mean_shift(*pa_in,
                                            ir,
                                            talus,
                                            GET("iterations", IntAttribute),
                                            GET("talus_weighted", BoolAttribute));
          },
          p_node->get_config_ref()->hmap_transform_mode_gpu);
    }
    else
    {
      hmap::transform(
          {p_out, p_in},
          [p_node, ir, talus](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_in = p_arrays[1];

            *pa_out = hmap::mean_shift(*pa_in,
                                       ir,
                                       talus,
                                       GET("iterations", IntAttribute),
                                       GET("talus_weighted", BoolAttribute));
          },
          p_node->get_config_ref()->hmap_transform_mode_cpu);
    }

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
