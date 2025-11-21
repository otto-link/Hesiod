/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_gamma_correction_local_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("radius", "radius", 0.05f, 0.01f, 0.2f);

  node.add_attr<FloatAttribute>("gamma", "gamma", 2.f, 0.01f, 10.f);

  node.add_attr<FloatAttribute>("k", "k", 0.1f, 0.f, 0.5f);

  node.add_attr<BoolAttribute>("GPU", "GPU", HSD_DEFAULT_GPU_MODE);

  // attribute(s) order
  node.set_attr_ordered_key({"radius", "gamma", "k", "_SEPARATOR_", "GPU"});
}

void compute_gamma_correction_local_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(node.get_attr<FloatAttribute>("radius") * p_out->shape.x));

    float hmin = p_out->min();
    float hmax = p_out->max();
    p_out->remap(0.f, 1.f, hmin, hmax);

    if (node.get_attr<BoolAttribute>("GPU"))
    {
      hmap::transform(
          {p_out, p_mask},
          [&node, &ir](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];

            hmap::gpu::gamma_correction_local(*pa_out,
                                              node.get_attr<FloatAttribute>("gamma"),
                                              ir,
                                              pa_mask,
                                              node.get_attr<FloatAttribute>("k"));
          },
          node.get_config_ref()->hmap_transform_mode_gpu);
    }
    else
    {
      hmap::transform(
          {p_out, p_mask},
          [&node, &ir](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];

            hmap::gamma_correction_local(*pa_out,
                                         node.get_attr<FloatAttribute>("gamma"),
                                         ir,
                                         pa_mask,
                                         node.get_attr<FloatAttribute>("k"));
          },
          node.get_config_ref()->hmap_transform_mode_cpu);
    }

    p_out->remap(hmin, hmax, 0.f, 1.f);
    p_out->smooth_overlap_buffers();
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
