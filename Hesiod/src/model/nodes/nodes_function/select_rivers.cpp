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

void setup_select_rivers_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "talus_ref", 0.1f, 0.01f, 10.f);
  ADD_ATTR(FloatAttribute, "clipping_ratio", 50.f, 0.1f, 100.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"talus_ref", "clipping_ratio"});

  setup_post_process_heightmap_attributes(p_node);
}

void compute_select_rivers_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // work on a single array as a temporary solution
    hmap::Array z_array = p_in->to_array();

    z_array = hmap::select_rivers(z_array,
                                  GET("talus_ref", FloatAttribute),
                                  GET("clipping_ratio", FloatAttribute));

    p_out->from_array_interp(z_array);

    // post-process
    post_process_heightmap(p_node, *p_out);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
