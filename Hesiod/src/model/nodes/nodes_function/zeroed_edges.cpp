/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/boundary.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_zeroed_edges_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dr");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("sigma", 2.f, 1.f, 4.f, "sigma");
  p_node->add_attr<MapEnumAttribute>("distance_function",
                                     "Euclidian",
                                     distance_function_map,
                                     "distance_function");
  p_node->add_attr<RangeAttribute>("remap_range", "remap_range");

  // attribute(s) order
  p_node->set_attr_ordered_key({"sigma", "distance_function", "remap_range"});
}

void compute_zeroed_edges_node(BaseNode *p_node)
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

    float sigma = GET("sigma", FloatAttribute);

    if (!p_dr)
      hmap::transform(*p_out,
                      [p_node, &sigma](hmap::Array &z, hmap::Vec4<float> bbox)
                      {
                        hmap::zeroed_edges(z,
                                           sigma,
                                           (hmap::DistanceFunction)
                                               GET("distance_function", MapEnumAttribute),
                                           nullptr,
                                           bbox);
                      });

    else
      hmap::transform(
          *p_out,
          *p_dr,
          [p_node, &sigma](hmap::Array &z, hmap::Array &dr, hmap::Vec4<float> bbox)
          {
            hmap::zeroed_edges(
                z,
                sigma,
                (hmap::DistanceFunction)GET("distance_function", MapEnumAttribute),
                &dr,
                bbox);
          });

    if (GET_ATTR("remap_range", RangeAttribute, is_active))
      p_out->remap(GET("remap_range", RangeAttribute)[0],
                   GET("remap_range", RangeAttribute)[1]);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
