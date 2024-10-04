/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_brush_node(BaseNode *p_node)
{
  LOG->trace("setup_brush_node");

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "out", CONFIG);

  // attribute(s)
  p_node->add_attr<ArrayAttribute>("hmap", "Heightmap", hmap::Vec2<int>(512, 512));
  p_node->add_attr<BoolAttribute>("inverse", false, "Inverse");
  p_node->add_attr<RangeAttribute>("remap", "Remap range");

  // attribute(s) order
  p_node->set_attr_ordered_key({"hmap", "inverse", "remap"});
}

void compute_brush_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  // base noise function
  hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("out");

  hmap::Array array = GET("hmap", ArrayAttribute);
  p_out->from_array_interp(array);

  // post-process
  post_process_heightmap(*p_out,
                         GET("inverse", BoolAttribute),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_ATTR("remap", RangeAttribute, is_active),
                         GET("remap", RangeAttribute));

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
