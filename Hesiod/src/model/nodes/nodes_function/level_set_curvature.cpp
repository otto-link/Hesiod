/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/curvature.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_level_set_curvature_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  std::vector<std::string> choices = {"positive", "negative", "both"};
  node.add_attr<ChoiceAttribute>("values_kept", "values_kept", choices, "both");
  node.add_attr<FloatAttribute>("radius", "radius", 0.1f, 0.f, 1.f, "{:.4f}");

  // attribute(s) order
  node.set_attr_ordered_key({"values_kept", "radius"});
}

void compute_level_set_curvature_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    int ir = (int)(node.get_attr<FloatAttribute>("radius") * p_out->shape.x);

    hmap::transform(
        {p_out, p_in},
        [&node, ir](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = hmap::gpu::level_set_curvature(*pa_in, ir);

          // determine curvature sign handling
          const std::string choice = node.get_attr<ChoiceAttribute>("values_kept");
          const bool        keep_both = (choice == "both");

          // if only one curvature sign is kept
          if (!keep_both)
          {
            if (choice == "negative")
              *pa_out *= -1.f;

            hmap::clamp_min(*pa_out, 0.f);
          }
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    p_out->smooth_overlap_buffers();
  }
}

} // namespace hesiod
