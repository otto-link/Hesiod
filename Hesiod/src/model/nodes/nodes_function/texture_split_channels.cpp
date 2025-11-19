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

void setup_texture_split_channels_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "R", CONFIG);
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "G", CONFIG);
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "B", CONFIG);
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "A", CONFIG);

  // attribute(s)

  // attribute(s) order
}

void compute_texture_split_channels_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::HeightmapRGBA *p_tex = node.get_value_ref<hmap::HeightmapRGBA>("texture");

  if (p_tex)
  {
    hmap::Heightmap *p_r = node.get_value_ref<hmap::Heightmap>("R");
    hmap::Heightmap *p_g = node.get_value_ref<hmap::Heightmap>("G");
    hmap::Heightmap *p_b = node.get_value_ref<hmap::Heightmap>("B");
    hmap::Heightmap *p_a = node.get_value_ref<hmap::Heightmap>("A");

    *p_r = p_tex->rgba[0];
    *p_g = p_tex->rgba[1];
    *p_b = p_tex->rgba[2];
    *p_a = p_tex->rgba[3];
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
