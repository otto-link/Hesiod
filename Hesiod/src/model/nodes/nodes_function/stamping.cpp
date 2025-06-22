/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/authoring.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_stamping_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Cloud>(gnode::PortType::IN, "cloud");
  p_node->add_port<hmap::Array>(gnode::PortType::IN, "kernel");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "kernel_radius", 0.1f, 0.01f, 0.5f);
  ADD_ATTR(BoolAttribute, "kernel_scale_radius", false);
  ADD_ATTR(BoolAttribute, "kernel_scale_amplitude", true);
  ADD_ATTR(EnumAttribute, "blend_method", stamping_blend_method_map, "maximum");
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(FloatAttribute, "k_smoothing", 0.1f, 0.01f, 1.f);
  ADD_ATTR(BoolAttribute, "kernel_flip", false);
  ADD_ATTR(BoolAttribute, "kernel_rotate", false);
  ADD_ATTR(BoolAttribute, "inverse", false);
  ADD_ATTR(RangeAttribute, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key({"kernel_radius",
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

void compute_stamping_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Cloud *p_cloud = p_node->get_value_ref<hmap::Cloud>("cloud");
  hmap::Array *p_kernel = p_node->get_value_ref<hmap::Array>("kernel");

  if (p_cloud && p_kernel)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    std::vector<float> xp = p_cloud->get_x();
    std::vector<float> yp = p_cloud->get_y();
    std::vector<float> zp = p_cloud->get_values();

    int  ir = std::max(1, (int)(GET("kernel_radius", FloatAttribute) * p_out->shape.x));
    uint seed = GET("seed", SeedAttribute);

    hmap::fill(*p_out,
               [p_node, &xp, &yp, &zp, p_kernel, ir, &seed](hmap::Vec2<int>   shape,
                                                            hmap::Vec4<float> bbox)
               {
                 return stamping(
                     shape,
                     xp,
                     yp,
                     zp,
                     *p_kernel,
                     ir,
                     GET("kernel_scale_radius", BoolAttribute),
                     GET("kernel_scale_amplitude", BoolAttribute),
                     (hmap::StampingBlendMethod)GET("blend_method", EnumAttribute),
                     seed++,
                     GET("k_smoothing", FloatAttribute),
                     GET("kernel_flip", BoolAttribute),
                     GET("kernel_rotate", BoolAttribute),
                     bbox);
               });

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(p_node,
                           *p_out,
                           GET("inverse", BoolAttribute),
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_MEMBER("remap", RangeAttribute, is_active),
                           GET("remap", RangeAttribute));
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
