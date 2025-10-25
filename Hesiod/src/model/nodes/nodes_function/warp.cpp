/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/transform.hpp"

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_warp_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "scaling.x", 1.f, -2.f, 2.f);
  ADD_ATTR(FloatAttribute, "scaling.y", 1.f, -2.f, 2.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"scaling.x", "scaling.y"});

  setup_post_process_heightmap_attributes(p_node, true);
}

void compute_warp_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  AppContext &ctx = HSD_CTX;

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
    hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    *p_out = *p_in;

    float sx = GET("scaling.x", FloatAttribute);
    float sy = GET("scaling.y", FloatAttribute);

    hmap::transform(
        {p_out, p_dx, p_dy},
        [sx, sy](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array dx = p_arrays[1] ? sx * (*p_arrays[1]) : hmap::Array(pa_out->shape);
          hmap::Array dy = p_arrays[2] ? sy * (*p_arrays[2]) : hmap::Array(pa_out->shape);

          hmap::gpu::warp(*pa_out, &dx, &dy);
        },
        ctx.app_settings.node_editor.hmap_transform_mode_gpu);

    // post-process
    post_process_heightmap(p_node, *p_out, p_in);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
