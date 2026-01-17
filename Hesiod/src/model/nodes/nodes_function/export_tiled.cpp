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

void setup_export_tiled_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "input");

  // attribute(s)
  node.add_attr<FilenameAttribute>("fname",
                                   "fname",
                                   std::filesystem::path("hmap.png"),
                                   "*",
                                   true);

  std::vector<std::string> choices = {"8 bit", "16 bit"};
  node.add_attr<ChoiceAttribute>("bit_depth", "PNG Bit Depth", choices, "16 bit");
  node.add_attr<IntAttribute>("tiling.x", "Nb. of Tiles (x)", 4, 1, INT_MAX);
  node.add_attr<IntAttribute>("tiling.y", "Nb. of Tiles (y)", 4, 1, INT_MAX);
  node.add_attr<IntAttribute>("leading_zeros", "Leading Zeroes", 3, 1, 6);
  node.add_attr<BoolAttribute>("overlapping_edges", "Overlapping Edges", false);
  node.add_attr<BoolAttribute>("reverse_tile_y_indexing", "Reverse y-indexing", false);
  node.add_attr<BoolAttribute>("auto_export", "auto_export", false);

  // attribute(s) order
  node.set_attr_ordered_key({"fname",
                             "bit_depth",
                             "tiling.x",
                             "tiling.y",
                             "leading_zeros",
                             "overlapping_edges",
                             "reverse_tile_y_indexing",
                             "auto_export"});
}

void compute_export_tiled_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualArray *p_in = node.get_value_ref<hmap::VirtualArray>("input");

  if (p_in && node.get_attr<BoolAttribute>("auto_export"))
  {
    // prepare parameters
    std::filesystem::path fname = node.get_attr<FilenameAttribute>("fname");

    int bit_depth;
    if (node.get_attr<ChoiceAttribute>("bit_depth") == "8 bit")
      bit_depth = CV_8U;
    else
      bit_depth = CV_16U;

    hmap::Vec2<int> tiling = {node.get_attr<IntAttribute>("tiling.x"),
                              node.get_attr<IntAttribute>("tiling.y")};

    // export
    hmap::export_tiled(fname.string(),
                       "png",
                       p_in->to_array(node.cfg().cm_cpu),
                       tiling,
                       node.get_attr<IntAttribute>("leading_zeros"),
                       bit_depth,
                       node.get_attr<BoolAttribute>("overlapping_edges"),
                       node.get_attr<BoolAttribute>("reverse_tile_y_indexing"));
  }
}

} // namespace hesiod
