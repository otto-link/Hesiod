/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/authoring.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_stamping_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Cloud>(gnode::PortType::IN, "cloud");
  node.add_port<hmap::Array>(gnode::PortType::IN, "kernel");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "kernel_radius", 0.1f, 0.01f, 0.5f);
  ADD_ATTR(node, BoolAttribute, "kernel_scale_radius", false);
  ADD_ATTR(node, BoolAttribute, "kernel_scale_amplitude", true);
  ADD_ATTR(node,
           EnumAttribute,
           "blend_method",
           enum_mappings.stamping_blend_method_map,
           "maximum");
  ADD_ATTR(node, SeedAttribute, "seed");
  ADD_ATTR(node, FloatAttribute, "k_smoothing", 0.1f, 0.01f, 1.f);
  ADD_ATTR(node, BoolAttribute, "kernel_flip", false);
  ADD_ATTR(node, BoolAttribute, "kernel_rotate", false);
  ADD_ATTR(node, BoolAttribute, "inverse", false);
  ADD_ATTR(node, RangeAttribute, "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"kernel_radius",
                             "kernel_scale_radius",
                             "kernel_scale_amplitude",
                             "blend_method",
                             "seed",
                             "k_smoothing",
                             "kernel_flip",
                             "kernel_rotate",
                             "_SEPARATOR_",
                             "inverse",
                             "remap"});
}

void compute_stamping_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Cloud *p_cloud = node.get_value_ref<hmap::Cloud>("cloud");
  hmap::Array *p_kernel = node.get_value_ref<hmap::Array>("kernel");

  if (p_cloud && p_kernel)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    std::vector<float> xp = p_cloud->get_x();
    std::vector<float> yp = p_cloud->get_y();
    std::vector<float> zp = p_cloud->get_values();

    int  ir = std::max(1,
                      (int)(GET(node, "kernel_radius", FloatAttribute) * p_out->shape.x));
    uint seed = GET(node, "seed", SeedAttribute);

    hmap::fill(*p_out,
               [&node, &xp, &yp, &zp, p_kernel, ir, &seed](hmap::Vec2<int>   shape,
                                                           hmap::Vec4<float> bbox)
               {
                 return stamping(
                     shape,
                     xp,
                     yp,
                     zp,
                     *p_kernel,
                     ir,
                     GET(node, "kernel_scale_radius", BoolAttribute),
                     GET(node, "kernel_scale_amplitude", BoolAttribute),
                     (hmap::StampingBlendMethod)GET(node, "blend_method", EnumAttribute),
                     seed++,
                     GET(node, "k_smoothing", FloatAttribute),
                     GET(node, "kernel_flip", BoolAttribute),
                     GET(node, "kernel_rotate", BoolAttribute),
                     bbox);
               });

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(node,
                           *p_out,
                           GET(node, "inverse", BoolAttribute),
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_MEMBER(node, "remap", RangeAttribute, is_active),
                           GET(node, "remap", RangeAttribute));
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
