/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_watershed_ridge_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "input");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "dx");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "dy");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "mask");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("amplitude", "Amplitude", 0.2f, 0.f, 2.f);
  node.add_attr<FloatAttribute>("edt_exponent", "Profile Exponent", 0.5f, 0.01f, 4.f);
  node.add_attr<EnumAttribute>("fd_method",
                               "Flow Direction Method",
                               enum_mappings.flow_direction_method_map);
  node.add_attr<FloatAttribute>("radius", "Smoothing radius", 0.05f, 0.f, 0.2f);

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Parameters",
                             "amplitude",
                             "edt_exponent",
                             "fd_method",
                             "radius",
                             "_GROUPBOX_END_"});

  setup_pre_process_mask_attributes(node);
  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = true, .remap_active_state = false});
}

void compute_watershed_ridge_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualArray *p_in = node.get_value_ref<hmap::VirtualArray>("input");

  if (p_in)
  {
    hmap::VirtualArray *p_dx = node.get_value_ref<hmap::VirtualArray>("dx");
    hmap::VirtualArray *p_dy = node.get_value_ref<hmap::VirtualArray>("dy");
    hmap::VirtualArray *p_mask = node.get_value_ref<hmap::VirtualArray>("mask");
    hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("output");

    // prepare mask
    std::shared_ptr<hmap::VirtualArray> sp_mask = pre_process_mask(node, p_mask, *p_in);

    int ir = std::max(1, (int)(node.get_attr<FloatAttribute>("radius") * p_out->shape.x));

    float hmin = p_in->min(node.cfg().cm_cpu);
    float hmax = p_in->max(node.cfg().cm_cpu);

    hmap::for_each_tile(
        {p_out, p_in, p_dx, p_dy, p_mask},
        [&node, ir](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &)
        {
          auto [pa_out, pa_in, pa_dx, pa_dy, pa_mask] = unpack<5>(p_arrays);

          auto fd_method = hmap::FlowDirectionMethod(
              node.get_attr<EnumAttribute>("fd_method"));

          *pa_out = hmap::gpu::watershed_ridge(
              *pa_in,
              pa_mask,
              node.get_attr<FloatAttribute>("amplitude"),
              ir,
              node.get_attr<FloatAttribute>("edt_exponent"),
              fd_method,
              pa_dx,
              pa_dy);
        },
        node.cfg().cm_single_array);

    p_out->remap(hmin, hmax, node.cfg().cm_cpu);

    // post-process
    p_out->smooth_overlap_buffers();
    post_process_heightmap(node, *p_out, p_in);
  }
}

} // namespace hesiod
