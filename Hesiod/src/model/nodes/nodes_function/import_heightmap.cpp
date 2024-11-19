/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_import_heightmap_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FilenameAttribute>("fname",
                                      "",
                                      false, // loading
                                      "Image files (*.bmp *.dib *.jpeg *.jpg *.png *.pbm "
                                      "*.pgm *.ppm *.pxm *.pnm *.tiff *.tif *.hdr *.pic)",
                                      "fname");
  p_node->add_attr<BoolAttribute>("remap", true, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key({"fname", "remap"});
}

void compute_import_heightmap_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

  std::ifstream f(GET("fname", FilenameAttribute).c_str());

  if (f.good())
  {
    hmap::Array z = hmap::Array(GET("fname", FilenameAttribute).string());

    p_out->from_array_interp_bicubic(z);

    if (GET("remap", BoolAttribute))
      p_out->remap();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
