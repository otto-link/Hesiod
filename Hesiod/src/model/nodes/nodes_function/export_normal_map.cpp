/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/export.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/nodes/post_process.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_export_normal_map_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");

  // attribute(s)
  ADD_ATTR(FilenameAttribute,
           "fname",
           std::filesystem::path("nmap.png"),
           "PNG (*.png)",
           true);
  ADD_ATTR(BoolAttribute, "16bit", false);
  ADD_ATTR(BoolAttribute, "auto_export", false);

  // attribute(s) order
  p_node->set_attr_ordered_key({"fname", "16bit", "auto_export"});

  // specialized GUI
  add_export_button(p_node);
}

void compute_export_normal_map_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in && GET("auto_export", BoolAttribute))
  {
    std::filesystem::path fname = GET("fname", FilenameAttribute);
    fname = ensure_extension(fname, ".png");

    if (GET("16bit", BoolAttribute))
      hmap::export_normal_map_png(fname.string(), p_in->to_array(), CV_16U);
    else
      hmap::export_normal_map_png(fname.string(), p_in->to_array(), CV_8U);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
