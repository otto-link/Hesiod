/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/export.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_export_as_cubemap_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");

  // attribute(s)
  ADD_ATTR(FilenameAttribute,
           "fname",
           std::filesystem::path("cubemap.png"),
           "PNG (*.png)",
           true);
  ADD_ATTR(IntAttribute, "cubemap_resolution", 64, 32, INT_MAX);
  ADD_ATTR(FloatAttribute, "overlap", 0.25f, 0.1f, 5.f);
  ADD_ATTR(IntAttribute, "ir", 16, 1, INT_MAX);
  ADD_ATTR(BoolAttribute, "splitted", false);
  ADD_ATTR(BoolAttribute, "auto_export", false);

  // attribute(s) order
  p_node->set_attr_ordered_key({"fname",
                                "cubemap_resolution",
                                "overlap",
                                "ir",
                                "splitted",
                                "_SEPARATOR_",
                                "auto_export"});
}

void compute_export_as_cubemap_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in && GET("auto_export", BoolAttribute))
  {
    hmap::Array z = p_in->to_array();

    std::filesystem::path fname = GET("fname", FilenameAttribute);
    fname = ensure_extension(fname, ".png").string();

    hmap::export_as_cubemap(fname,
                            z,
                            GET("cubemap_resolution", IntAttribute),
                            GET("overlap", FloatAttribute),
                            GET("ir", IntAttribute),
                            hmap::Cmap::GRAY,
                            GET("splitted", BoolAttribute),
                            nullptr);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
