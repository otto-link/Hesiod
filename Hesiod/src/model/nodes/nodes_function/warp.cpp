/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/transform.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_warp_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<BoolAttribute>("GPU", HSD_DEFAULT_GPU_MODE, "GPU");
}

void compute_warp_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
    hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    if (GET("GPU", BoolAttribute))
    {
      hmap::transform(
          {p_out, p_dx, p_dy},
          [&ir](std::vector<hmap::Array *> p_arrays, hmap::Vec2<int>, hmap::Vec4<float>)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_dx = p_arrays[1];
            hmap::Array *pa_dy = p_arrays[2];

            hmap::gpu::warp(*pa_out, *pa_dx, *pa_dy);
          },
          p_node->get_config_ref()->hmap_transform_mode_gpu);
    }
    else
    {
      hmap::transform(
          {p_out, p_dx, p_dy},
          [&ir](std::vector<hmap::Array *> p_arrays, hmap::Vec2<int>, hmap::Vec4<float>)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_dx = p_arrays[1];
            hmap::Array *pa_dy = p_arrays[2];

            hmap::warp(*pa_out, *pa_dx, *pa_dy);
          },
          p_node->get_config_ref()->hmap_transform_mode_cpu);
    }

    p_out->from_array_interp_nearest(z_array);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
