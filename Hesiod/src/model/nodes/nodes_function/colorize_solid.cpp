/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_colorize_solid_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::OUT, "texture", CONFIG);

  // attribute(s)
  p_node->add_attr<ColorAttribute>("color", 1.f, 1.f, 1.f, 1.f, "color");
  p_node->add_attr<FloatAttribute>("alpha", 1.f, 0.f, 1.f, "alpha");

  // attribute(s) order
  p_node->set_attr_ordered_key({"color", "alpha"});
}

void compute_colorize_solid_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMapRGBA *p_out = p_node->get_value_ref<hmap::HeightMapRGBA>("texture");
  std::vector<float>   col3 = GET("color", ColorAttribute);

  for (int k = 0; k < 4; k++)
  {
    float color = k < 3 ? col3[k] : GET("alpha", FloatAttribute);
    p_out->rgba[k] = hmap::HeightMap(CONFIG, color);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod