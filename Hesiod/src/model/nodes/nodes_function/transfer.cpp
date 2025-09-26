/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/blending.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_transfer_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "source");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "target");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "radius", 0.05f, 0.f, 0.2f);
  ADD_ATTR(FloatAttribute, "amplitude", -1.f, -2.f, 4.f);
  ADD_ATTR(BoolAttribute, "target_prefiltering", true);

  // attribute(s) order
  p_node->set_attr_ordered_key({"radius", "amplitude", "target_prefiltering"});

  setup_pre_process_mask_attributes(p_node);
  setup_post_process_heightmap_attributes(p_node);
}

void compute_transfer_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_s = p_node->get_value_ref<hmap::Heightmap>("source");
  hmap::Heightmap *p_t = p_node->get_value_ref<hmap::Heightmap>("target");

  if (p_s && p_t)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    int ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));

    hmap::transform(
        {p_out, p_s, p_t},
        [p_node, ir](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_s = p_arrays[1];
          hmap::Array *pa_t = p_arrays[2];

          *pa_out = hmap::gpu::transfer(*pa_s,
                                        *pa_t,
                                        ir,
                                        GET("amplitude", FloatAttribute),
                                        GET("target_prefiltering", BoolAttribute));
        },
        p_node->get_config_ref()->hmap_transform_mode_gpu);

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(p_node, *p_out);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
