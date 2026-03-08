/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/export.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_export_heightmap_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "input");

  // attribute(s)
  node.add_attr<FilenameAttribute>("fname",
                                   "Filename",
                                   std::filesystem::path("hmap.png"),
                                   "*",
                                   true);
  node.add_attr<EnumAttribute>("format",
                               "File Format",
                               enum_mappings.heightmap_export_format_map,
                               "png (16 bit)");
  node.add_attr<BoolAttribute>("auto_export", "Auto Export on Node Update", false);
  node.add_attr<BoolAttribute>("add_prefix", "Add Project Name as Prefix", false);

  // attribute(s) order
  node.set_attr_ordered_key({"fname", "format", "auto_export", "add_prefix"});
}

void compute_export_heightmap_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualArray *p_in = node.get_value_ref<hmap::VirtualArray>("input");

  if (p_in && node.get_attr<BoolAttribute>("auto_export"))
  {
    std::filesystem::path fname = node.get_attr<FilenameAttribute>("fname");

    if (node.get_attr<BoolAttribute>("add_prefix"))
      fname = prepend_project_name_to_path(fname);

    switch (node.get_attr<EnumAttribute>("format"))
    {
    case ExportFormat::PNG8BIT:
    {
      fname = ensure_extension(fname, ".png");
      p_in->to_array(node.cfg().cm_cpu).to_png_grayscale(fname.string(), CV_8U);
    }
    break;

    case ExportFormat::PNG16BIT:
    {
      fname = ensure_extension(fname, ".png");
      p_in->to_array(node.cfg().cm_cpu).to_png_grayscale(fname.string(), CV_16U);
    }
    break;

    case ExportFormat::RAW16BIT:
    {
      fname = ensure_extension(fname, ".raw");
      p_in->to_array(node.cfg().cm_cpu).to_raw_16bit(fname.string());
    }
    break;
    }
  }
}

} // namespace hesiod
