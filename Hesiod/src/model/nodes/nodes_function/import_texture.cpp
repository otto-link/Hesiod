/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "highmap/tensor.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_import_texture_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "texture", CONFIG(node));

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
}

void compute_import_texture_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::HeightmapRGBA *p_out = node.get_value_ref<hmap::HeightmapRGBA>("texture");

  std::string fname = node.get_attr<FilenameAttribute>("fname").string();

  // if the file exists, keep going
  std::ifstream f(fname.c_str());
  if (f.good())
  {
    // load rgba data
    hmap::Tensor tensor4(fname, node.get_attr<BoolAttribute>("flip_y"));
    tensor4 = tensor4.resample_to_shape_xy(node.get_config_ref()->shape);

    hmap::Heightmap r(CONFIG(node));
    hmap::Heightmap g(CONFIG(node));
    hmap::Heightmap b(CONFIG(node));
    hmap::Heightmap a(CONFIG(node));

    hmap::Array ra = tensor4.get_slice(0);
    hmap::Array ga = tensor4.get_slice(1);
    hmap::Array ba = tensor4.get_slice(2);
    hmap::Array aa = tensor4.get_slice(3);

    r.from_array_interp(ra);
    g.from_array_interp(ga);
    b.from_array_interp(ba);
    a.from_array_interp(aa);

    *p_out = hmap::HeightmapRGBA(r, g, b, a);
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
