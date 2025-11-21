/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/gradient.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_texture_to_heightmap_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "elevation", CONFIG(node));

  // attribute(s)

  // attribute(s) order
  setup_post_process_heightmap_attributes(node);
}

void compute_texture_to_heightmap_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::HeightmapRGBA *p_tex = node.get_value_ref<hmap::HeightmapRGBA>("texture");

  if (p_tex)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("elevation");
    *p_out = p_tex->luminance();

    // post-process
    post_process_heightmap(node, *p_out);
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
