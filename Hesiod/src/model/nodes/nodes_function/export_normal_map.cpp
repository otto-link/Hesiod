/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/export.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
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
  p_node->add_attr<FilenameAttribute>("fname",
                                      "nmap.png",
                                      true, // save
                                      "PNG (*.png)",
                                      "fname");
  p_node->add_attr<BoolAttribute>("16bit", false, "16bit");
  p_node->add_attr<BoolAttribute>("auto_export", true, "auto_export");

  // attribute(s) order
  p_node->set_attr_ordered_key({"fname", "16bit", "auto_export"});
}

void compute_export_normal_map_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in && GET("auto_export", BoolAttribute))
  {
    std::string fname = GET("fname", FilenameAttribute).string();

    if (GET("16bit", BoolAttribute))
      hmap::export_normal_map_png(fname, p_in->to_array(), CV_16U);
    else
      hmap::export_normal_map_png(fname, p_in->to_array(), CV_8U);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
