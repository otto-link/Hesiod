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

void setup_recast_canyon_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "noise");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("vcut", "vcut", 0.5f, -1.f, 2.f);

  node.add_attr<FloatAttribute>("gamma", "gamma", 4.f, 0.01f, 10.f);

  // attribute(s) order
  node.set_attr_ordered_key({"vcut", "gamma"});
}

void compute_recast_canyon_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_noise = node.get_value_ref<hmap::Heightmap>("noise");
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    hmap::transform(*p_out,
                    p_noise,
                    p_mask,
                    [&node](hmap::Array &z, hmap::Array *p_noise, hmap::Array *p_mask)
                    {
                      hmap::recast_canyon(z,
                                          node.get_attr<FloatAttribute>("vcut"),
                                          p_mask,
                                          node.get_attr<FloatAttribute>("gamma"),
                                          p_noise);
                    });
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
