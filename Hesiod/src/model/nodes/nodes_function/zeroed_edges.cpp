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

void setup_zeroed_edges_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dr");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("sigma", "Falloff Exponent", 2.f, 1.f, 4.f);
  node.add_attr<EnumAttribute>("distance_function",
                               "Distance Function:",
                               enum_mappings.distance_function_map,
                               "Euclidian");

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Main Parameters",
                             "sigma",
                             "distance_function",
                             "_GROUPBOX_END_"});

  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = true, .remap_active_state = false});
}

void compute_zeroed_edges_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_dr = node.get_value_ref<hmap::Heightmap>("dr");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    float sigma = node.get_attr<FloatAttribute>("sigma");

    hmap::transform(
        {p_out, p_in, p_dr},
        [&node, sigma](std::vector<hmap::Array *> p_arrays,
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
              (hmap::DistanceFunction)node.get_attr<EnumAttribute>("distance_function"),
              pa_dr,
              bbox);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    // post-process
    post_process_heightmap(node, *p_out);
  }
}

} // namespace hesiod
