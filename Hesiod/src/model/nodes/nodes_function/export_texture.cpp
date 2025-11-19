/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/export.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/nodes/post_process.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_export_texture_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture");

  // attribute(s)
  ADD_ATTR(node,
           FilenameAttribute,
           "fname",
           std::filesystem::path("texture.png"),
           "PNG (*.png)",
           true);
  ADD_ATTR(node, BoolAttribute, "16 bit", false);
  ADD_ATTR(node, BoolAttribute, "auto_export", false);

  // attribute(s) order
  node.set_attr_ordered_key({"fname", "16 bit", "auto_export"});

  // specialized GUI
  add_export_button(node);
}

void compute_export_texture_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::HeightmapRGBA *p_in = node.get_value_ref<hmap::HeightmapRGBA>("texture");

  if (p_in && GET(node, "auto_export", BoolAttribute))
  {
    std::filesystem::path fname = GET(node, "fname", FilenameAttribute);
    fname = ensure_extension(fname, ".png");

    if (GET(node, "16 bit", BoolAttribute))
      p_in->to_png(fname.string(), CV_16U);
    else
      p_in->to_png(fname.string(), CV_8U);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
