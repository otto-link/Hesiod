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

void setup_select_transitions_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input 1");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input 2");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "blend");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  setup_post_process_heightmap_attributes(node);
}

void compute_select_transitions_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in1 = node.get_value_ref<hmap::Heightmap>("input 1");
  hmap::Heightmap *p_in2 = node.get_value_ref<hmap::Heightmap>("input 2");
  hmap::Heightmap *p_blend = node.get_value_ref<hmap::Heightmap>("blend");

  if (p_in1 && p_in2 && p_blend)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    hmap::transform(*p_out,
                    *p_in1,
                    *p_in2,
                    *p_blend,
                    [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2, hmap::Array &a3)
                    { m = hmap::select_transitions(a1, a2, a3); });

    // post-process
    post_process_heightmap(node, *p_out);
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
