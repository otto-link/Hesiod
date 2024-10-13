/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/export.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

void setup_export_heightmap_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");

  // attribute(s)
  p_node->add_attr<FilenameAttribute>("fname", "hmap.png", "", "Filename");
  p_node->add_attr<MapEnumAttribute>("format",
                                     "png (8 bit)",
                                     heightmap_export_format_map,
                                     "Export format");
  p_node->add_attr<BoolAttribute>("auto_export", true, "Automatic export");

  // attribute(s) order
  p_node->set_attr_ordered_key({"fname", "format", "auto_export"});
}

void compute_export_heightmap_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in && GET("auto_export", BoolAttribute))
  {
    std::string fname = GET("fname", FilenameAttribute).string();

    switch (GET("format", MapEnumAttribute))
    {
    case ExportFormat::PNG8BIT:
      p_in->to_array().to_png_grayscale(fname, CV_8U);
      break;

    case ExportFormat::PNG16BIT:
      p_in->to_array().to_png_grayscale(fname, CV_16U);
      break;

    case ExportFormat::RAW16BIT:
      p_in->to_array().to_raw_16bit(fname);
      break;
    }
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
