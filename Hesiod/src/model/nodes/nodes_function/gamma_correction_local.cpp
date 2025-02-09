/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_gamma_correction_local_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_attr<BoolAttribute>("GPU", HSD_DEFAULT_GPU_MODE, "GPU");

  // attribute(s)
  p_node->add_attr<FloatAttribute>("radius", 0.05f, 0.01f, 0.2f, "radius");
  p_node->add_attr<FloatAttribute>("gamma", 2.f, 0.01f, 10.f, "gamma");
  p_node->add_attr<FloatAttribute>("k", 0.1f, 0.f, 0.5f, "k");

  // attribute(s) order
  p_node->set_attr_ordered_key({"radius", "gamma", "k", "_SEPARATOR_", "GPU"});
}

void compute_gamma_correction_local_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));

    float hmin = p_out->min();
    float hmax = p_out->max();
    p_out->remap(0.f, 1.f, hmin, hmax);

    if (GET("GPU", BoolAttribute))
    {
      hmap::transform(
          {p_out, p_mask},
          [p_node,
           &ir](std::vector<hmap::Array *> p_arrays, hmap::Vec2<int>, hmap::Vec4<float>)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];

            hmap::gpu::gamma_correction_local(*pa_out,
                                              GET("gamma", FloatAttribute),
                                              ir,
                                              pa_mask,
                                              GET("k", FloatAttribute));
          },
          p_node->get_config_ref()->hmap_transform_mode_gpu);
    }
    else
    {
      hmap::transform(
          {p_out, p_mask},
          [p_node,
           &ir](std::vector<hmap::Array *> p_arrays, hmap::Vec2<int>, hmap::Vec4<float>)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];

            hmap::gamma_correction_local(*pa_out,
                                         GET("gamma", FloatAttribute),
                                         ir,
                                         pa_mask,
                                         GET("k", FloatAttribute));
          },
          p_node->get_config_ref()->hmap_transform_mode_cpu);
    }

    p_out->remap(hmin, hmax, 0.f, 1.f);
    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
