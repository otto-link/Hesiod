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
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node,
           FilenameAttribute,
           "fname",
           std::filesystem::path(""),
           "Image files (*.bmp *.dib *.jpeg *.jpg *.png *.pbm "
           "*.pgm *.ppm *.pxm *.pnm *.tiff *.tif *.hdr *.pic)",
           false);
  ADD_ATTR(node, BoolAttribute, "flip_y", true);

  // attribute(s) order
  node.set_attr_ordered_key({"fname", "flip_y"});

  setup_post_process_heightmap_attributes(node);
}

void compute_import_heightmap_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  const std::string fname = GET(node, "fname", FilenameAttribute).string();
  std::ifstream     f(fname.c_str());

  if (f.good())
  {
    hmap::Array z = hmap::Array(GET(node, "fname", FilenameAttribute).string(),
                                GET(node, "flip_y", BoolAttribute));
    p_out->from_array_interp_bicubic(z);

    // post-process
    post_process_heightmap(node, *p_out);
  }
  else
  {
    Logger::log()->error("compute_import_heightmap_node: could not load image file {}",
                         fname);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
