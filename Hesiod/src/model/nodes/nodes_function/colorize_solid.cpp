/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

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
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "texture", CONFIG(node));

  // attribute(s)
  node.add_attr<ColorAttribute>("color", "color", 0.5f, 0.5f, 0.5f, 1.f);
  node.add_attr<FloatAttribute>("alpha", "alpha", 1.f, 0.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"color", "alpha"});
}

void compute_colorize_solid_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::HeightmapRGBA *p_out = node.get_value_ref<hmap::HeightmapRGBA>("texture");
  std::vector<float>   col3 = node.get_attr<ColorAttribute>("color");

  for (int k = 0; k < 4; k++)
  {
    float color = k < 3 ? col3[k] : node.get_attr<FloatAttribute>("alpha");
    p_out->rgba[k] = hmap::Heightmap(CONFIG(node), color);
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
