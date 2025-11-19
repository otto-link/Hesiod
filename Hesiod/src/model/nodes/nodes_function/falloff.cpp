/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * p_node software. */
#include "highmap/boundary.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_falloff_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dr");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "strength", 1.f, 0.f, 4.f);
  ADD_ATTR(node,
           EnumAttribute,
           "distance_function",
           enum_mappings.distance_function_map,
           "Euclidian");
  ADD_ATTR(node, RangeAttribute, "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"strength", "distance_function", "remap"});
}

void compute_falloff_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_dr = node.get_value_ref<hmap::Heightmap>("dr");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    float strength = GET(node, "strength", FloatAttribute);

    if (!p_dr)
      hmap::transform(*p_out,
                      [&node, &strength](hmap::Array &z, hmap::Vec4<float> bbox)
                      {
                        hmap::falloff(z,
                                      strength,
                                      (hmap::DistanceFunction)GET(node,
                                                                  "distance_function",
                                                                  EnumAttribute),
                                      nullptr,
                                      bbox);
                      });

    else
      hmap::transform(
          *p_out,
          *p_dr,
          [&node, &strength](hmap::Array &z, hmap::Array &dr, hmap::Vec4<float> bbox)
          {
            hmap::falloff(
                z,
                strength,
                (hmap::DistanceFunction)GET(node, "distance_function", EnumAttribute),
                &dr,
                bbox);
          });

    if (GET_MEMBER(node, "remap", RangeAttribute, is_active))
      p_out->remap(GET(node, "remap", RangeAttribute)[0],
                   GET(node, "remap", RangeAttribute)[1]);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
