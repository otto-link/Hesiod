/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/gradient.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_texture_to_heightmap_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "elevation", CONFIG);

  // attribute(s)

  // attribute(s) order
  setup_post_process_heightmap_attributes(p_node);
}

void compute_texture_to_heightmap_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  AppContext &ctx = HSD_CTX;

  hmap::HeightmapRGBA *p_tex = p_node->get_value_ref<hmap::HeightmapRGBA>("texture");

  if (p_tex)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("elevation");
    *p_out = p_tex->luminance();

    // post-process
    post_process_heightmap(p_node, *p_out);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
