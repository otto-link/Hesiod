/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/virtual_array/virtual_texture.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_colorize_solid_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualTexture>(gnode::PortType::OUT, "texture", CONFIG_TEX(node));

  // attribute(s)
  node.add_attr<ColorAttribute>("color", "color", 0.5f, 0.5f, 0.5f, 1.f);
  node.add_attr<FloatAttribute>("alpha", "alpha", 1.f, 0.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"color", "alpha"});
}

void compute_colorize_solid_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualTexture *p_tex = node.get_value_ref<hmap::VirtualTexture>("texture");
  std::vector<float>    col3 = node.get_attr<ColorAttribute>("color");

  for (int nch = 0; nch < 4; ++nch)
  {
    float v = nch < 3 ? col3[nch] : node.get_attr<FloatAttribute>("alpha");
    p_tex->fill(nch, v, node.cfg().cm_cpu);
  }
}

} // namespace hesiod
