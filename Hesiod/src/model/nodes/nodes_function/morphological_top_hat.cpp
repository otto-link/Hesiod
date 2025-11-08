/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/morphology.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_morphological_top_hat_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "radius", 0.01f, 0.f, 0.05f);
  ADD_ATTR(BoolAttribute, "top_hat", "top_hat", "black_hat", true);

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"_GROUPBOX_BEGIN_Main parameters", "radius", "top_hat", "_GROUPBOX_END_"});

  setup_post_process_heightmap_attributes(p_node);
}

void compute_morphological_top_hat_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    int ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));

    hmap::transform(
        {p_out, p_in},
        [p_node, ir](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          if (GET("top_hat", BoolAttribute))
            *pa_out = hmap::gpu::morphological_top_hat(*pa_in, ir);
          else
            *pa_out = hmap::gpu::morphological_black_hat(*pa_in, ir);
        },
        p_node->get_config_ref()->hmap_transform_mode_gpu);

    // post-process
    p_out->smooth_overlap_buffers();
    post_process_heightmap(p_node, *p_out);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
