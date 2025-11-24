/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_recast_cliff_directional_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("angle", "angle", 45.f, -180.f, 180.f);
  node.add_attr<FloatAttribute>("talus_global", "talus_global", 1.f, 0.f, 5.f);
  node.add_attr<FloatAttribute>("radius", "radius", 0.1f, 0.01f, 0.5f);
  node.add_attr<FloatAttribute>("amplitude", "amplitude", 0.1f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("gain", "gain", 2.f, 0.01f, 10.f);

  // attribute(s) order
  node.set_attr_ordered_key({"angle", "talus_global", "radius", "amplitude", "gain"});
}

void compute_recast_cliff_directional_node(BaseNode &node)
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
    int ir = std::max(1, (int)(node.get_attr<FloatAttribute>("radius") * p_out->shape.x));

    hmap::transform(*p_out,
                    p_mask,
                    [&node, &talus, &ir](hmap::Array &z, hmap::Array *p_mask)
                    {
                      hmap::recast_cliff_directional(
                          z,
                          talus,
                          ir,
                          node.get_attr<FloatAttribute>("amplitude"),
                          node.get_attr<FloatAttribute>("angle"),
                          p_mask,
                          node.get_attr<FloatAttribute>("gain"));
                    });

    p_out->smooth_overlap_buffers();
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
