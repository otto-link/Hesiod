/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/blending.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_blend_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input 1");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input 2");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<MapEnumAttribute>("blending_method",
                                     "minimum_smooth",
                                     blending_method_map,
                                     "blending_method");
  p_node->add_attr<FloatAttribute>("k", 0.1f, 0.01f, 1.f, "k");
  p_node->add_attr<FloatAttribute>("radius", 0.05f, 0.f, 0.2f, "radius");
  p_node->add_attr<BoolAttribute>("inverse", false, "inverse");
  p_node->add_attr<RangeAttribute>("remap_range", "remap_range");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"blending_method", "k", "radius", "_SEPARATOR_", "inverse", "remap_range"});
}

void compute_blend_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in1 = p_node->get_value_ref<hmap::Heightmap>("input 1");
  hmap::Heightmap *p_in2 = p_node->get_value_ref<hmap::Heightmap>("input 2");

  if (p_in1 && p_in2)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    std::function<void(hmap::Array &, hmap::Array &, hmap::Array &)> lambda;

    float k = GET("k", FloatAttribute);
    int   ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));
    int   method = GET("blending_method", MapEnumAttribute);

    switch (method)
    {
    case BlendingMethod::ADD:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2) { m = a1 + a2; };
      break;

    case BlendingMethod::EXCLUSION_BLEND:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::blend_exclusion(a1, a2); };
      break;

    case BlendingMethod::GRADIENTS:
      lambda = [&ir](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::blend_gradients(a1, a2, ir); };
      break;

    case BlendingMethod::MAXIMUM:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::maximum(a1, a2); };
      break;

    case BlendingMethod::MAXIMUM_SMOOTH:
      lambda = [&k](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::maximum_smooth(a1, a2, k); };
      break;

    case BlendingMethod::MINIMUM:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::minimum(a1, a2); };
      break;

    case BlendingMethod::MINIMUM_SMOOTH:
      lambda = [&k](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::minimum_smooth(a1, a2, k); };
      break;

    case BlendingMethod::MULTIPLY:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2) { m = a1 * a2; };
      break;

    case BlendingMethod::MULTIPLY_ADD:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2) { m = a1 + a1 * a2; };
      break;

    case BlendingMethod::NEGATE:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::blend_negate(a1, a2); };
      break;

    case BlendingMethod::OVERLAY:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::blend_overlay(a1, a2); };
      break;

    case BlendingMethod::SOFT:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::blend_soft(a1, a2); };
      break;

    case BlendingMethod::SUBSTRACT:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2) { m = a1 - a2; };
      break;
    }

    hmap::transform(*p_out, *p_in1, *p_in2, lambda);

    if (method == BlendingMethod::GRADIENTS)
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
