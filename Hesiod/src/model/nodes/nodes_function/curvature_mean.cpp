/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/curvature.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_curvature_mean_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  std::vector<std::string> choices = {"positive", "negative", "both"};
  ADD_ATTR(ChoiceAttribute, "values_kept", choices);

  ADD_ATTR(BoolAttribute, "clamp_max", false);
  ADD_ATTR(FloatAttribute, "vc_max", 1.f, 0.f, FLT_MAX, "{:.4f}");

  // attribute(s) order
  p_node->set_attr_ordered_key({"values_kept", "clamp_max", "vc_max"});

  setup_post_process_heightmap_attributes(p_node);
}

void compute_curvature_mean_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  AppContext &ctx = HSD_CTX;

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    int nx = p_out->shape.x; // for gradient scaling

    hmap::transform(
        {p_out, p_in},
        [p_node, nx](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          // compute mean curvature and scale it
          *pa_out = hmap::curvature_mean(*pa_in) * nx;

          // determine curvature sign handling
          const std::string choice = GET("values_kept", ChoiceAttribute);
          const bool        keep_both = (choice == "both");

          // if only one curvature sign is kept
          if (!keep_both)
          {
            if (choice == "negative")
              *pa_out *= -1.f;

            hmap::clamp_min(*pa_out, 0.f);
          }

          // clamp output range
          const float vc_max = GET("vc_max", FloatAttribute);

          if (GET("clamp_max", BoolAttribute) && !keep_both)
            hmap::clamp_max(*pa_out, vc_max);
          else
            hmap::clamp(*pa_out, -vc_max, vc_max);
        },
        ctx.app_settings.node_editor.hmap_transform_mode_cpu);

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(p_node, *p_out);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
