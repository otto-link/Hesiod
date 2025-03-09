/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * p_node software. */
#include "highmap/boundary.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_falloff_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dr");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "strength", 1.f, 0.f, 4.f);
  ADD_ATTR(EnumAttribute, "distance_function", distance_function_map, "Euclidian");
  ADD_ATTR(RangeAttribute, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key({"strength", "distance_function", "remap"});
}

void compute_falloff_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_dr = p_node->get_value_ref<hmap::Heightmap>("dr");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    float strength = GET("strength", FloatAttribute);

    if (!p_dr)
      hmap::transform(
          *p_out,
          [p_node, &strength](hmap::Array &z, hmap::Vec4<float> bbox)
          {
            hmap::falloff(z,
                          strength,
                          (hmap::DistanceFunction)GET("distance_function", EnumAttribute),
                          nullptr,
                          bbox);
          });

    else
      hmap::transform(
          *p_out,
          *p_dr,
          [p_node, &strength](hmap::Array &z, hmap::Array &dr, hmap::Vec4<float> bbox)
          {
            hmap::falloff(z,
                          strength,
                          (hmap::DistanceFunction)GET("distance_function", EnumAttribute),
                          &dr,
                          bbox);
          });

    if (GET_ATTR("remap", RangeAttribute, is_active))
      p_out->remap(GET("remap", RangeAttribute)[0], GET("remap", RangeAttribute)[1]);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
