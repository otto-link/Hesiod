/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/boundary.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_zeroed_edges_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dr");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "sigma", 2.f, 1.f, 4.f);
  ADD_ATTR(EnumAttribute,
           "distance_function",
           enum_mappings.distance_function_map,
           "Euclidian");
  ADD_ATTR(RangeAttribute, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key({"sigma", "distance_function", "remap"});
}

void compute_zeroed_edges_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // AppContext &ctx = HSD_CTX;

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_dr = p_node->get_value_ref<hmap::Heightmap>("dr");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    float sigma = GET("sigma", FloatAttribute);

    hmap::transform(
        {p_out, p_in, p_dr},
        [p_node, sigma](std::vector<hmap::Array *> p_arrays,
                        hmap::Vec2<int>,
                        hmap::Vec4<float> bbox)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];
          hmap::Array *pa_dr = p_arrays[2];

          *pa_out = *pa_in;

          hmap::zeroed_edges(
              *pa_out,
              sigma,
              (hmap::DistanceFunction)GET("distance_function", EnumAttribute),
              pa_dr,
              bbox);
        },
        HSD_CPU_MODE);

    // post-process
    post_process_heightmap(p_node,
                           *p_out,
                           false, // inverse
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
