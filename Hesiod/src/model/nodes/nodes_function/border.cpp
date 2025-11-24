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

void setup_border_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("radius", "radius", 0.01f, 0.f, 0.05f);
  node.add_attr<RangeAttribute>("remap", "remap");
  node.add_attr<BoolAttribute>("GPU", "GPU", HSD_DEFAULT_GPU_MODE);

  // attribute(s) order
  node.set_attr_ordered_key({"radius", "remap", "_SEPARATOR_", "GPU"});
}

void compute_border_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    int ir = std::max(1, (int)(node.get_attr<FloatAttribute>("radius") * p_out->shape.x));

    if (node.get_attr<BoolAttribute>("GPU"))
    {
      hmap::transform(*p_out,
                      *p_in,
                      [&ir](hmap::Array &out, hmap::Array &in)
                      { out = hmap::gpu::border(in, ir); });
    }
    else
    {
      hmap::transform(*p_out,
                      *p_in,
                      [&ir](hmap::Array &out, hmap::Array &in)
                      { out = hmap::border(in, ir); });
    }

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(node,
                           *p_out,
                           false, // inverse
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           node.get_attr_ref<RangeAttribute>("remap")->get_is_active(),
                           node.get_attr<RangeAttribute>("remap"));
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
