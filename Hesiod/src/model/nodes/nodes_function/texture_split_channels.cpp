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

void setup_texture_split_channels_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "R", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "G", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "B", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "A", CONFIG);

  // attribute(s)

  // attribute(s) order
}

void compute_texture_split_channels_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::HeightmapRGBA *p_tex = p_node->get_value_ref<hmap::HeightmapRGBA>("texture");

  if (p_tex)
  {
    hmap::Heightmap *p_r = p_node->get_value_ref<hmap::Heightmap>("R");
    hmap::Heightmap *p_g = p_node->get_value_ref<hmap::Heightmap>("G");
    hmap::Heightmap *p_b = p_node->get_value_ref<hmap::Heightmap>("B");
    hmap::Heightmap *p_a = p_node->get_value_ref<hmap::Heightmap>("A");

    *p_r = p_tex->rgba[0];
    *p_g = p_tex->rgba[1];
    *p_b = p_tex->rgba[2];
    *p_a = p_tex->rgba[3];
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
