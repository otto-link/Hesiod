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
  node.add_attr<FilenameAttribute>("fname",
                                   "fname",
                                   std::filesystem::path("texture.png"),
                                   "PNG (*.png)",
                                   true);
  node.add_attr<BoolAttribute>("16 bit", "16 bit", false);

  node.add_attr<BoolAttribute>("auto_export", "auto_export", false);

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

  if (p_in && node.get_attr<BoolAttribute>("auto_export"))
  {
    std::filesystem::path fname = node.get_attr<FilenameAttribute>("fname");
    fname = ensure_extension(fname, ".png");

    if (node.get_attr<BoolAttribute>("16 bit"))
      p_in->to_png(fname.string(), CV_16U);
    else
      p_in->to_png(fname.string(), CV_8U);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
