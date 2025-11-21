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
  node.add_attr<FilenameAttribute>("fname",
                                   "fname",
                                   std::filesystem::path("hmap.png"),
                                   "*",
                                   true);
  node.add_attr<EnumAttribute>("format",
                               "format",
                               enum_mappings.heightmap_export_format_map,
                               "png (16 bit)");
  node.add_attr<BoolAttribute>("auto_export", "auto_export", false);

  // attribute(s) order
  node.set_attr_ordered_key({"fname", "format", "auto_export"});

  // specialized GUI
  add_export_button(node);
}

void compute_export_heightmap_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in && node.get_attr<BoolAttribute>("auto_export"))
  {
    std::filesystem::path fname = node.get_attr<FilenameAttribute>("fname");

    switch (node.get_attr<EnumAttribute>("format"))
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

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
