/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/blending.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_blend_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input 1");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input 2");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(EnumAttribute,
           "blending_method",
           HSD_CTX.enum_mappings.blending_method_map,
           "minimum_smooth");
  ADD_ATTR(FloatAttribute, "k", 0.1f, 0.01f, 1.f);
  ADD_ATTR(FloatAttribute, "radius", 0.05f, 0.f, 0.2f);
  ADD_ATTR(BoolAttribute, "inverse", false);
  ADD_ATTR(RangeAttribute, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"blending_method", "k", "radius", "_SEPARATOR_", "inverse", "remap"});
}

void compute_blend_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in1 = p_node->get_value_ref<hmap::Heightmap>("input 1");
  hmap::Heightmap *p_in2 = p_node->get_value_ref<hmap::Heightmap>("input 2");

  if (p_in1 && p_in2)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    float k = GET("k", FloatAttribute);
    int   ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));
    int   method = GET("blending_method", EnumAttribute);

    blend_heightmaps(*p_out, *p_in1, *p_in2, static_cast<BlendingMethod>(method), k, ir);

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
