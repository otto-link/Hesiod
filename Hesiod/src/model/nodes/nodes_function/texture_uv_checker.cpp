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

void setup_texture_uv_checker_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "texture", CONFIG(node));

  // attribute(s)
  std::vector<std::string> choices = {"4x4", "8x8", "16x16"};
  node.add_attr<ChoiceAttribute>("size", "size", choices, "8x8");
}

void compute_texture_uv_checker_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::HeightmapRGBA *p_out = node.get_value_ref<hmap::HeightmapRGBA>("texture");

  // generated with https://uvchecker.vinzi.xyz/

  std::string fname;

  if (node.get_attr<ChoiceAttribute>("size") == "4x4")
    fname = "data/uv_checker_2k_04x04.png";
  else if (node.get_attr<ChoiceAttribute>("size") == "8x8")
    fname = "data/uv_checker_2k_08x08.png";
  else if (node.get_attr<ChoiceAttribute>("size") == "16x16")
    fname = "data/uv_checker_2k_16x16.png";

  // if the file exists, keep going
  std::ifstream f(fname.c_str());
  if (f.good())
  {
    // load rgba data
    bool         flip_j = true;
    hmap::Tensor tensor4(fname, flip_j);
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
