/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/selector.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_select_multiband3_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "low", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "mid", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "high", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("ratio1", "ratio1", 0.2f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("ratio2", "ratio2", 0.5f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("overlap", "overlap", 0.5f, 0.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"ratio1", "ratio2", "overlap"});

  setup_post_process_heightmap_attributes(node);
}

void compute_select_multiband3_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_low = node.get_value_ref<hmap::Heightmap>("low");
    hmap::Heightmap *p_mid = node.get_value_ref<hmap::Heightmap>("mid");
    hmap::Heightmap *p_high = node.get_value_ref<hmap::Heightmap>("high");

    float vmin = p_in->min();
    float vmax = p_in->max();

    hmap::transform(*p_in,
                    *p_low,
                    *p_mid,
                    *p_high,
                    [&node, vmin, vmax](hmap::Array &in,
                                        hmap::Array &low,
                                        hmap::Array &mid,
                                        hmap::Array &high)
                    {
                      hmap::select_multiband3(in,
                                              low,
                                              mid,
                                              high,
                                              node.get_attr<FloatAttribute>("ratio1"),
                                              node.get_attr<FloatAttribute>("ratio2"),
                                              node.get_attr<FloatAttribute>("overlap"),
                                              vmin,
                                              vmax);
                    });

    // post-process
    post_process_heightmap(node, *p_low);
    post_process_heightmap(node, *p_mid);
    post_process_heightmap(node, *p_high);
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
