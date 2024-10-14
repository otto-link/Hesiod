/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "highmap/tensor.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_import_texture_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::OUT, "texture", CONFIG);

  // attribute(s)
  p_node->add_attr<FilenameAttribute>("fname",
                                      "",
                                      "Image files (*.bmp *.dib *.jpeg *.jpg *.png *.pbm "
                                      "*.pgm *.ppm *.pxm *.pnm *.tiff *.tif *.hdr *.pic)",
                                      "fname");
}

void compute_import_texture_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMapRGBA *p_out = p_node->get_value_ref<hmap::HeightMapRGBA>("texture");

  std::string fname = GET("fname", FilenameAttribute).string();

  // if the file exists, keep going
  std::ifstream f(fname.c_str());
  if (f.good())
  {
    // load rgba data
    hmap::Tensor tensor4(fname);
    tensor4 = tensor4.resample_to_shape_xy(p_node->get_config_ref()->shape);

    hmap::HeightMap r(CONFIG);
    hmap::HeightMap g(CONFIG);
    hmap::HeightMap b(CONFIG);
    hmap::HeightMap a(CONFIG);

    hmap::Array ra = tensor4.get_slice(0);
    hmap::Array ga = tensor4.get_slice(1);
    hmap::Array ba = tensor4.get_slice(2);
    hmap::Array aa = tensor4.get_slice(3);

    r.from_array_interp(ra);
    g.from_array_interp(ga);
    b.from_array_interp(ba);
    a.from_array_interp(aa);

    *p_out = hmap::HeightMapRGBA(r, g, b, a);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
