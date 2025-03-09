/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/features.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_relative_elevation_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "radius", 0.05f, 0.f, 0.2f);
  ADD_ATTR(BoolAttribute, "GPU", HSD_DEFAULT_GPU_MODE);

  // attribute(s) order
  p_node->set_attr_ordered_key({"radius", "_SEPARATOR_", "GPU"});
}

void compute_relative_elevation_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));

    if (GET("GPU", BoolAttribute))
    {
      hmap::transform(
          {p_out},
          [&ir](std::vector<hmap::Array *> p_arrays, hmap::Vec2<int>, hmap::Vec4<float>)
          {
            hmap::Array *pa_out = p_arrays[0];
            *pa_out = hmap::gpu::relative_elevation(*pa_out, ir);
          },
          p_node->get_config_ref()->hmap_transform_mode_gpu);
    }
    else
    {
      hmap::transform(
          {p_out},
          [&ir](std::vector<hmap::Array *> p_arrays, hmap::Vec2<int>, hmap::Vec4<float>)
          {
            hmap::Array *pa_out = p_arrays[0];
            *pa_out = hmap::relative_elevation(*pa_out, ir);
          },
          p_node->get_config_ref()->hmap_transform_mode_cpu);
    }

    p_out->smooth_overlap_buffers();
    p_out->remap();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
