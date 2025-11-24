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
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("strength", "strength", 1.f, 0.f, 4.f);
  node.add_attr<EnumAttribute>("distance_function",
                               "distance_function",
                               enum_mappings.distance_function_map,
                               "Euclidian");
  node.add_attr<RangeAttribute>("remap", "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"strength", "distance_function", "remap"});
}

void compute_falloff_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_dr = node.get_value_ref<hmap::Heightmap>("dr");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    float strength = node.get_attr<FloatAttribute>("strength");

    if (!p_dr)
      hmap::transform(
          *p_out,
          [&node, &strength](hmap::Array &z, hmap::Vec4<float> bbox)
          {
            hmap::falloff(
                z,
                strength,
                (hmap::DistanceFunction)node.get_attr<EnumAttribute>("distance_function"),
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
                (hmap::DistanceFunction)node.get_attr<EnumAttribute>("distance_function"),
                &dr,
                bbox);
          });

    if (node.get_attr_ref<RangeAttribute>("remap")->get_is_active())
      p_out->remap(node.get_attr<RangeAttribute>("remap")[0],
                   node.get_attr<RangeAttribute>("remap")[1]);
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
