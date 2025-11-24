/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_import_heightmap_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FilenameAttribute>("fname",
                                   "fname",
                                   std::filesystem::path(""),
                                   "Image files (*.bmp *.dib *.jpeg *.jpg *.png *.pbm "
                                   "*.pgm *.ppm *.pxm *.pnm *.tiff *.tif *.hdr *.pic)",
                                   false);
  node.add_attr<BoolAttribute>("flip_y", "flip_y", true);

  // attribute(s) order
  node.set_attr_ordered_key({"fname", "flip_y"});

  setup_post_process_heightmap_attributes(node);
}

void compute_import_heightmap_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  const std::string fname = node.get_attr<FilenameAttribute>("fname").string();
  std::ifstream     f(fname.c_str());

  if (f.good())
  {
    hmap::Array z = hmap::Array(node.get_attr<FilenameAttribute>("fname").string(),
                                node.get_attr<BoolAttribute>("flip_y"));
    p_out->from_array_interp_bicubic(z);

    // post-process
    post_process_heightmap(node, *p_out);
  }
  else
  {
    Logger::log()->error("compute_import_heightmap_node: could not load image file {}",
                         fname);
  }
}

} // namespace hesiod
