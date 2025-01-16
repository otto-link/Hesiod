/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/authoring.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

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
  p_node->add_attr<FloatAttribute>("kernel_radius", 0.1f, 0.01f, 0.5f, "kernel_radius");
  p_node->add_attr<BoolAttribute>("kernel_scale_radius", false, "kernel_scale_radius");
  p_node->add_attr<BoolAttribute>("kernel_scale_amplitude",
                                  true,
                                  "kernel_scale_amplitude");
  p_node->add_attr<MapEnumAttribute>("blend_method",
                                     "maximum",
                                     stamping_blend_method_map,
                                     "blend_method");
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<FloatAttribute>("k_smoothing", 0.1f, 0.01f, 1.f, "k_smoothing");
  p_node->add_attr<BoolAttribute>("kernel_flip", false, "kernel_flip");
  p_node->add_attr<BoolAttribute>("kernel_rotate", false, "kernel_rotate");
  p_node->add_attr<BoolAttribute>("inverse", false, "inverse");
  p_node->add_attr<RangeAttribute>("remap_range", "remap_range");

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
                                "remap_range"});
}

void compute_stamping_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

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
                     (hmap::StampingBlendMethod)GET("blend_method", MapEnumAttribute),
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
                           GET_ATTR("remap_range", RangeAttribute, is_active),
                           GET("remap_range", RangeAttribute));
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
