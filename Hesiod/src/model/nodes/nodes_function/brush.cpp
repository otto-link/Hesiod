/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_brush_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG);

  // attribute(s)
  p_node->add_attr<ArrayAttribute>("hmap", "Heightmap", hmap::Vec2<int>(512, 512));

  // attribute(s) order
  p_node->set_attr_ordered_key({"hmap"});

  setup_post_process_heightmap_attributes(p_node);
}

void compute_brush_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // base noise function
  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("out");

  hmap::Array array = GET("hmap", ArrayAttribute);
  p_out->from_array_interp(array);

  // post-process
  post_process_heightmap(p_node, *p_out);

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
