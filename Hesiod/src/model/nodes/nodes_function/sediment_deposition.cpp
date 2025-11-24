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

void setup_sediment_deposition_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("talus_global", "talus_global", 0.2f, 0.f, FLT_MAX);
  node.add_attr<FloatAttribute>("max_deposition", "max_deposition", 0.001f, 0.f, 0.1f);
  node.add_attr<IntAttribute>("iterations", "iterations", 1, 1, 20);
  node.add_attr<IntAttribute>("thermal_subiterations",
                              "thermal_subiterations",
                              50,
                              1,
                              INT_MAX);
  node.add_attr<BoolAttribute>("scale_talus_with_elevation",
                               "scale_talus_with_elevation",
                               true);

  // attribute(s) order
  node.set_attr_ordered_key({"talus_global",
                             "max_deposition",
                             "iterations",
                             "thermal_subiterations",
                             "scale_talus_with_elevation"});
}

void compute_sediment_deposition_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_deposition_map = node.get_value_ref<hmap::Heightmap>("deposition");

    // copy the input heightmap
    *p_out = *p_in;

    float talus = node.get_attr<FloatAttribute>("talus_global") / (float)p_out->shape.x;

    hmap::Heightmap talus_map = hmap::Heightmap(CONFIG(node), talus);

    if (node.get_attr<BoolAttribute>("scale_talus_with_elevation"))
    {
      talus_map = *p_in;
      talus_map.remap(talus / 100.f, talus);
    }

    hmap::transform(*p_out,
                    p_mask,
                    &talus_map,
                    p_deposition_map,
                    [&node, &talus](hmap::Array &h_out,
                                    hmap::Array *p_mask_array,
                                    hmap::Array *p_talus_array,
                                    hmap::Array *p_deposition_array)
                    {
                      hmap::sediment_deposition(
                          h_out,
                          p_mask_array,
                          *p_talus_array,
                          p_deposition_array,
                          node.get_attr<FloatAttribute>("max_deposition"),
                          node.get_attr<IntAttribute>("iterations"),
                          node.get_attr<IntAttribute>("thermal_subiterations"));
                    });

    p_out->smooth_overlap_buffers();

    if (p_deposition_map)
    {
      p_deposition_map->smooth_overlap_buffers();
      p_deposition_map->remap();
    }
  }
}

} // namespace hesiod
