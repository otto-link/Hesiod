/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/curvature.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_unsphericity_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("radius", "radius", 0.01f, 0.f, 0.2f);
  node.add_attr<BoolAttribute>("remap", "remap", true);
  node.add_attr<BoolAttribute>("inverse", "inverse", false);
  node.add_attr<BoolAttribute>("smoothing", "smoothing", false);
  node.add_attr<FloatAttribute>("smoothing_radius", "smoothing_radius", 0.05f, 0.f, 0.2f);
  node.add_attr<BoolAttribute>("GPU", "GPU", HSD_DEFAULT_GPU_MODE);

  // attribute(s) order
  node.set_attr_ordered_key({"radius",
                             "inverse",
                             "remap",
                             "smoothing",
                             "smoothing_radius",
                             "_SEPARATOR_",
                             "GPU"});
}

void compute_unsphericity_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // zero radius accepted
    int ir = std::max(0, (int)(node.get_attr<FloatAttribute>("radius") * p_out->shape.x));

    if (node.get_attr<BoolAttribute>("GPU"))
    {
      hmap::transform(
          {p_out, p_in},
          [&ir](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_in = p_arrays[1];

            *pa_out = hmap::gpu::unsphericity(*pa_in, ir);
          },
          node.get_config_ref()->hmap_transform_mode_gpu);
    }
    else
    {
      hmap::transform(
          {p_out, p_in},
          [&ir](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_in = p_arrays[1];

            *pa_out = hmap::unsphericity(*pa_in, ir);
          },
          node.get_config_ref()->hmap_transform_mode_cpu);
    }

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(node,
                           *p_out,
                           node.get_attr<BoolAttribute>("inverse"),
                           node.get_attr<BoolAttribute>("smoothing"),
                           node.get_attr<FloatAttribute>("smoothing_radius"),
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           node.get_attr<BoolAttribute>("remap"),
                           {0.f, 1.f});
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
