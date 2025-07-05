/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/export.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_export_heightmap_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");

  // attribute(s)
  ADD_ATTR(FilenameAttribute, "fname", std::filesystem::path("hmap.png"), "*", true);
  ADD_ATTR(EnumAttribute, "format", heightmap_export_format_map, "png (8 bit)");
  ADD_ATTR(BoolAttribute, "auto_export", false);

  // attribute(s) order
  p_node->set_attr_ordered_key({"fname", "format", "auto_export"});

  // specialized GUI
  add_export_button(p_node);
}

void compute_export_heightmap_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in && GET("auto_export", BoolAttribute))
  {
    std::filesystem::path fname = GET("fname", FilenameAttribute);

    switch (GET("format", EnumAttribute))
    {
    case ExportFormat::PNG8BIT:
    {
      fname = ensure_extension(fname, ".png").string();
      p_in->to_array().to_png_grayscale(fname, CV_8U);
    }
    break;

    case ExportFormat::PNG16BIT:
    {
      fname = ensure_extension(fname, ".png").string();
      p_in->to_array().to_png_grayscale(fname, CV_16U);
    }
    break;

    case ExportFormat::RAW16BIT:
    {
      fname = ensure_extension(fname, ".raw").string();
      p_in->to_array().to_raw_16bit(fname);
    }
    break;
    }
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
