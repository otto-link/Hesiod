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

void setup_transfer_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "source");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "target");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("radius", "radius", 0.05f, 0.f, 0.2f);

  node.add_attr<FloatAttribute>("amplitude", "amplitude", 0.5f, -2.f, 4.f);

  node.add_attr<BoolAttribute>("target_prefiltering", "target_prefiltering", false);

  // attribute(s) order
  node.set_attr_ordered_key({"radius", "amplitude", "target_prefiltering"});

  setup_pre_process_mask_attributes(node);
  setup_post_process_heightmap_attributes(node);
}

void compute_transfer_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_s = node.get_value_ref<hmap::Heightmap>("source");
  hmap::Heightmap *p_t = node.get_value_ref<hmap::Heightmap>("target");

  if (p_s && p_t)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    int ir = std::max(1, (int)(node.get_attr<FloatAttribute>("radius") * p_out->shape.x));

    hmap::transform(
        {p_out, p_s, p_t},
        [&node, ir](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_s = p_arrays[1];
          hmap::Array *pa_t = p_arrays[2];

          *pa_out = hmap::gpu::transfer(
              *pa_s,
              *pa_t,
              ir,
              node.get_attr<FloatAttribute>("amplitude"),
              node.get_attr<BoolAttribute>("target_prefiltering"));
        },
        node.get_config_ref()->hmap_transform_mode_gpu);

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(node, *p_out);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
