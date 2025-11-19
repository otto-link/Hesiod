/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/export.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_export_heightmap_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");

  // attribute(s)
  ADD_ATTR(node,
           FilenameAttribute,
           "fname",
           std::filesystem::path("hmap.png"),
           "*",
           true);
  ADD_ATTR(node,
           EnumAttribute,
           "format",
           enum_mappings.heightmap_export_format_map,
           "png (16 bit)");
  ADD_ATTR(node, BoolAttribute, "auto_export", false);

  // attribute(s) order
  node.set_attr_ordered_key({"fname", "format", "auto_export"});

  // specialized GUI
  add_export_button(node);
}

void compute_export_heightmap_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in && GET(node, "auto_export", BoolAttribute))
  {
    std::filesystem::path fname = GET(node, "fname", FilenameAttribute);

    switch (GET(node, "format", EnumAttribute))
    {
    case ExportFormat::PNG8BIT:
    {
      fname = ensure_extension(fname, ".png");
      p_in->to_array().to_png_grayscale(fname.string(), CV_8U);
    }
    break;

    case ExportFormat::PNG16BIT:
    {
      fname = ensure_extension(fname, ".png");
      p_in->to_array().to_png_grayscale(fname.string(), CV_16U);
    }
    break;

    case ExportFormat::RAW16BIT:
    {
      fname = ensure_extension(fname, ".raw");
      p_in->to_array().to_raw_16bit(fname.string());
    }
    break;
    }
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
