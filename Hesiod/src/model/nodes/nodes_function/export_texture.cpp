/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/export.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_export_texture_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture");

  // attribute(s)
  p_node->add_attr<FilenameAttribute>("fname",
                                      "texture.png",
                                      true, // save
                                      "PNG (*.png)",
                                      "fname");
  p_node->add_attr<BoolAttribute>("16 bit", false, "16 bit");
  p_node->add_attr<BoolAttribute>("auto_export", true, "auto_export");

  // attribute(s) order
  p_node->set_attr_ordered_key({"fname", "16 bit", "auto_export"});
}

void compute_export_texture_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightmapRGBA *p_in = p_node->get_value_ref<hmap::HeightmapRGBA>("texture");

  if (p_in && GET("auto_export", BoolAttribute))
  {
    if (GET("16 bit", BoolAttribute))
      p_in->to_png(GET("fname", FilenameAttribute).string(), CV_16U);
    else
      p_in->to_png(GET("fname", FilenameAttribute).string(), CV_8U);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
