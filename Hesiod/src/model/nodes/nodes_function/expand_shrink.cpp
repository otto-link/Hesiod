/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/kernels.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_expand_shrink_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Array>(gnode::PortType::IN, "kernel");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, EnumAttribute, "kernel", enum_mappings.kernel_type_map, "cubic_pulse");
  ADD_ATTR(node, FloatAttribute, "radius", 0.05f, 0.01f, 0.2f);
  ADD_ATTR(node, BoolAttribute, "shrink", "shrink", "expand", false);
  ADD_ATTR(node, IntAttribute, "iterations", 1, 1, 10);

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Main Parameters",
                             "kernel",
                             "radius",
                             "shrink",
                             "iterations",
                             "_GROUPBOX_END_"});

  setup_pre_process_mask_attributes(node);
  setup_post_process_heightmap_attributes(node, true);
}

void compute_expand_shrink_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // prepare mask
    std::shared_ptr<hmap::Heightmap> sp_mask = pre_process_mask(node, p_mask, *p_in);

    // copy the input heightmap
    *p_out = *p_in;

    // kernel definition - use input kernel by default, if not switch
    // to built-in kernels
    hmap::Array  kernel_array;
    hmap::Array *p_kernel = node.get_value_ref<hmap::Array>("kernel");

    if (p_kernel)
    {
      kernel_array = *p_kernel;
    }
    else
    {
      int ir = std::max(1, (int)(GET(node, "radius", FloatAttribute) * p_out->shape.x));
      hmap::Vec2<int> kernel_shape = {2 * ir + 1, 2 * ir + 1};

      kernel_array = hmap::get_kernel(
          kernel_shape,
          (hmap::KernelType)GET(node, "kernel", EnumAttribute));
    }

    // core operator
    std::function<void(hmap::Array &, hmap::Array *)> lambda;

    if (GET(node, "shrink", BoolAttribute))
    {
      hmap::transform(
          {p_out, p_mask},
          [&node, &kernel_array](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];
            hmap::gpu::shrink(*pa_out,
                              kernel_array,
                              pa_mask,
                              GET(node, "iterations", IntAttribute));
          },
          node.get_config_ref()->hmap_transform_mode_gpu);
    }
    else
    {
      hmap::transform(
          {p_out, p_mask},
          [&node, &kernel_array](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];
            hmap::gpu::expand(*pa_out,
                              kernel_array,
                              pa_mask,
                              GET(node, "iterations", IntAttribute));
          },
          node.get_config_ref()->hmap_transform_mode_gpu);
    }

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(node, *p_out, p_in);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
