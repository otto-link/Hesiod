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

void setup_export_as_cubemap_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");

  // attribute(s)
  ADD_ATTR(node,
           FilenameAttribute,
           "fname",
           std::filesystem::path("cubemap.png"),
           "PNG (*.png)",
           true);
  ADD_ATTR(node, IntAttribute, "cubemap_resolution", 64, 32, INT_MAX);
  ADD_ATTR(node, FloatAttribute, "overlap", 0.25f, 0.1f, 5.f);
  ADD_ATTR(node, IntAttribute, "ir", 16, 1, INT_MAX);
  ADD_ATTR(node, BoolAttribute, "splitted", false);
  ADD_ATTR(node, BoolAttribute, "auto_export", false);

  // attribute(s) order
  node.set_attr_ordered_key({"fname",
                             "cubemap_resolution",
                             "overlap",
                             "ir",
                             "splitted",
                             "_SEPARATOR_",
                             "auto_export"});

  // specialized GUI
  add_export_button(node);
}

void compute_export_as_cubemap_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in && GET(node, "auto_export", BoolAttribute))
  {
    hmap::Array z = p_in->to_array();

    std::filesystem::path fname = GET(node, "fname", FilenameAttribute);
    fname = ensure_extension(fname, ".png");

    hmap::export_as_cubemap(fname.string(),
                            z,
                            GET(node, "cubemap_resolution", IntAttribute),
                            GET(node, "overlap", FloatAttribute),
                            GET(node, "ir", IntAttribute),
                            hmap::Cmap::GRAY,
                            GET(node, "splitted", BoolAttribute),
                            nullptr);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
