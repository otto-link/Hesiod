/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/kernels.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_expand_shrink_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Array>(gnode::PortType::IN, "kernel");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(EnumAttribute, "kernel", kernel_type_map, "cubic_pulse");
  ADD_ATTR(FloatAttribute, "radius", 0.05f, 0.01f, 0.2f);
  ADD_ATTR(BoolAttribute, "shrink", "shrink", "expand", false);

  // attribute(s) order
  p_node->set_attr_ordered_key({"kernel", "radius", "shrink"});

  setup_pre_process_mask_attributes(p_node);
  setup_post_process_heightmap_attributes(p_node, true);
}

void compute_expand_shrink_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // prepare mask
    std::shared_ptr<hmap::Heightmap> sp_mask = pre_process_mask(p_node, p_mask, *p_in);

    // copy the input heightmap
    *p_out = *p_in;

    // kernel definition - use input kernel by default, if not switch
    // to built-in kernels
    hmap::Array  kernel_array;
    hmap::Array *p_kernel = p_node->get_value_ref<hmap::Array>("kernel");

    if (p_kernel)
    {
      kernel_array = *p_kernel;
    }
    else
    {
      int ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));
      hmap::Vec2<int> kernel_shape = {2 * ir + 1, 2 * ir + 1};

      kernel_array = hmap::get_kernel(kernel_shape,
                                      (hmap::KernelType)GET("kernel", EnumAttribute));
    }

    // core operator
    std::function<void(hmap::Array &, hmap::Array *)> lambda;

    if (GET("shrink", BoolAttribute))
    {
      hmap::transform(
          {p_out, p_mask},
          [&kernel_array](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];
            hmap::gpu::shrink(*pa_out, kernel_array, pa_mask);
          },
          p_node->get_config_ref()->hmap_transform_mode_gpu);
    }
    else
    {
      hmap::transform(
          {p_out, p_mask},
          [&kernel_array](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];
            hmap::gpu::expand(*pa_out, kernel_array, pa_mask);
          },
          p_node->get_config_ref()->hmap_transform_mode_gpu);
    }

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(p_node, *p_out, p_in);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
