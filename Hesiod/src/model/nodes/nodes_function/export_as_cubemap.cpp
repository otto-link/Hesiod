/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/export.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
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
  node.add_attr<FilenameAttribute>("fname",
                                   "fname",
                                   std::filesystem::path("cubemap.png"),
                                   "PNG (*.png)",
                                   true);
  node.add_attr<IntAttribute>("cubemap_resolution",
                              "cubemap_resolution",
                              64,
                              32,
                              INT_MAX);
  node.add_attr<FloatAttribute>("overlap", "overlap", 0.25f, 0.1f, 5.f);
  node.add_attr<IntAttribute>("ir", "ir", 16, 1, INT_MAX);
  node.add_attr<BoolAttribute>("splitted", "splitted", false);
  node.add_attr<BoolAttribute>("auto_export", "auto_export", false);

  // attribute(s) order
  node.set_attr_ordered_key({"fname",
                             "cubemap_resolution",
                             "overlap",
                             "ir",
                             "splitted",
                             "_SEPARATOR_",
                             "auto_export"});

  // specialized GUI
}

void compute_export_as_cubemap_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in && node.get_attr<BoolAttribute>("auto_export"))
  {
    hmap::Array z = p_in->to_array();

    std::filesystem::path fname = node.get_attr<FilenameAttribute>("fname");
    fname = ensure_extension(fname, ".png");

    hmap::export_as_cubemap(fname.string(),
                            z,
                            node.get_attr<IntAttribute>("cubemap_resolution"),
                            node.get_attr<FloatAttribute>("overlap"),
                            node.get_attr<IntAttribute>("ir"),
                            hmap::Cmap::GRAY,
                            node.get_attr<BoolAttribute>("splitted"),
                            nullptr);
  }
}

} // namespace hesiod
