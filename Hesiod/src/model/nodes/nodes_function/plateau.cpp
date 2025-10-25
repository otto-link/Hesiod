/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_plateau_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "radius", 0.05f, 0.01f, 0.5f);
  ADD_ATTR(FloatAttribute, "factor", 4.f, 0.01f, 10.f);
  ADD_ATTR(BoolAttribute, "GPU", HSD_DEFAULT_GPU_MODE);

  // attribute(s) order
  p_node->set_attr_ordered_key({"radius", "factor", "_SEPARATOR_", "GPU"});
}

void compute_plateau_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  AppContext &ctx = HSD_CTX;

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));

    if (GET("GPU", BoolAttribute))
    {
      hmap::transform(
          {p_out, p_mask},
          [p_node, &ir](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];

            hmap::gpu::plateau(*pa_out, pa_mask, ir, GET("factor", FloatAttribute));
          },
          ctx.app_settings.node_editor.hmap_transform_mode_gpu);
    }
    else
    {
      hmap::transform(
          {p_out, p_mask},
          [p_node, &ir](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];

            hmap::plateau(*pa_out, pa_mask, ir, GET("factor", FloatAttribute));
          },
          ctx.app_settings.node_editor.hmap_transform_mode_cpu);
    }

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
