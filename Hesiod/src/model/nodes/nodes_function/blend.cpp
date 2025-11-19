/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/blending.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_blend_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input 1");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input 2");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  node.add_attr<EnumAttribute>("blending_method",
                               "Method:",
                               enum_mappings.blending_method_map,
                               "minimum_smooth");

  ADD_ATTR(node, FloatAttribute, "k", 0.1f, 0.01f, 1.f);
  ADD_ATTR(node, FloatAttribute, "radius", 0.05f, 0.f, 0.2f);
  ADD_ATTR(node, FloatAttribute, "input1_weight", 1.f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "input2_weight", 1.f, 0.f, 1.f);
  ADD_ATTR(node, BoolAttribute, "swap_inputs", false);
  ADD_ATTR(node, BoolAttribute, "inverse", false);
  ADD_ATTR(node, RangeAttribute, "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Main Parameters",
                             "blending_method",
                             "k",
                             "radius",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Inputs",
                             "input1_weight",
                             "input2_weight",
                             "swap_inputs",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Post-processing",
                             "inverse",
                             "remap",
                             "_GROUPBOX_END_"});
}

void compute_blend_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in1 = node.get_value_ref<hmap::Heightmap>("input 1");
  hmap::Heightmap *p_in2 = node.get_value_ref<hmap::Heightmap>("input 2");

  if (p_in1 && p_in2)
  {
    // adjust inputs
    if (GET(node, "swap_inputs", BoolAttribute))
      std::swap(p_in1, p_in2);

    // compute output
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    float k = GET(node, "k", FloatAttribute);
    int   ir = std::max(1, (int)(GET(node, "radius", FloatAttribute) * p_out->shape.x));
    int   method = GET(node, "blending_method", EnumAttribute);

    blend_heightmaps(*p_out,
                     *p_in1,
                     *p_in2,
                     static_cast<BlendingMethod>(method),
                     k,
                     ir,
                     GET(node, "input1_weight", FloatAttribute),
                     GET(node, "input2_weight", FloatAttribute));

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
