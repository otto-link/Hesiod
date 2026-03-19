/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "highmap/range.hpp"

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
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FilenameAttribute>("fname",
                                   "fname",
                                   std::filesystem::path(""),
                                   "Image files (*.bmp *.dib *.jpeg *.jpg *.png *.pbm "
                                   "*.pgm *.ppm *.pxm *.pnm *.tiff *.tif *.hdr *.pic)",
                                   false);
  node.add_attr<BoolAttribute>("flip_y", "flip_y", true);

  node.add_attr<BoolAttribute>("clip_range", "clip_range", true);

  std::vector<std::string> choices = {"Bicubic", "Nearest"};
  node.add_attr<ChoiceAttribute>("sampling_method", "Sampling Method", choices);

  // attribute(s) order
  node.set_attr_ordered_key({"fname", "flip_y", "clip_range", "sampling_method"});

  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = false, .remap_active_state = true});
}

void compute_import_heightmap_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("output");

  const std::string fname = node.get_attr<FilenameAttribute>("fname").string();

  // sanity check
  {
    std::ifstream f(fname.c_str());

    if (!f.good())
    {
      Logger::log()->error("compute_import_heightmap_node: could not load image file {}",
                           fname);
      return;
    }
  }

  // raw import
  hmap::Array z = hmap::Array(fname, node.get_attr<BoolAttribute>("flip_y"));

  // resample to current shape
  const std::string sampling_method = node.get_attr<ChoiceAttribute>("sampling_method");

  if (sampling_method == "Bicubic")
    z = z.resample_to_shape_bicubic(p_out->shape);
  else if (sampling_method == "Nearest")
    z = z.resample_to_shape_nearest(p_out->shape);

  p_out->from_array(z, node.cfg().cm_cpu);

  // make sure value range remains within [0, 1] to avoid overshoots
  // due to interpolation
  if (node.get_attr<BoolAttribute>("clip_range"))
  {
    hmap::for_each_tile(
        {p_out},
        [](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &)
        {
          auto [pa_out] = unpack<1>(p_arrays);
          hmap::clamp(*pa_out, 0.f, 1.f);
        },
        node.cfg().cm_cpu);
  }

  // post-process
  post_process_heightmap(node, *p_out);
}

} // namespace hesiod
