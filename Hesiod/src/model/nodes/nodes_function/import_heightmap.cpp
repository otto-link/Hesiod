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

void setup_import_heightmap_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FilenameAttribute,
           "fname",
           std::filesystem::path(""),
           "Image files (*.bmp *.dib *.jpeg *.jpg *.png *.pbm "
           "*.pgm *.ppm *.pxm *.pnm *.tiff *.tif *.hdr *.pic)",
           false);

  // attribute(s) order
  p_node->set_attr_ordered_key({"fname"});

  setup_post_process_heightmap_attributes(p_node);
}

void compute_import_heightmap_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

  const std::string fname = GET("fname", FilenameAttribute).string();
  std::ifstream     f(fname.c_str());

  if (f.good())
  {
    hmap::Array z = hmap::Array(GET("fname", FilenameAttribute).string());
    p_out->from_array_interp_bicubic(z);

    // post-process
    post_process_heightmap(p_node, *p_out);
  }
  else
  {
    Logger::log()->error("compute_import_heightmap_node: could not load image file {}",
                         fname);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
