/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_thermal_schott_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("talus_global", "talus_global", 1.f, 0.f, FLT_MAX);
  node.add_attr<IntAttribute>("iterations", "iterations", 10, 1, INT_MAX);
  node.add_attr<BoolAttribute>("scale_talus_with_elevation",
                               "scale_talus_with_elevation",
                               false);

  // attribute(s) order
  node.set_attr_ordered_key({"talus_global", "iterations", "scale_talus_with_elevation"});
}

void compute_thermal_schott_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    float talus = node.get_attr<FloatAttribute>("talus_global") / (float)p_out->shape.x;
    float intensity = 0.05f * talus;

    hmap::Heightmap talus_map = hmap::Heightmap(CONFIG(node), talus);

    if (node.get_attr<BoolAttribute>("scale_talus_with_elevation"))
    {
      talus_map = *p_in;
      talus_map.remap(talus / 100.f, talus);
    }

    hmap::transform(*p_out,
                    p_mask,
                    &talus_map,
                    [&node, intensity](hmap::Array &h_out,
                                       hmap::Array *p_mask_array,
                                       hmap::Array *p_talus_array)
                    {
                      hmap::thermal_schott(h_out,
                                           *p_talus_array,
                                           p_mask_array,
                                           node.get_attr<IntAttribute>("iterations"),
                                           intensity);
                    });

    p_out->smooth_overlap_buffers();
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
