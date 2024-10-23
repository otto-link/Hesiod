/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/cmap.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_set_alpha_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::IN, "texture in");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "alpha");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "noise");
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::OUT, "texture out", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("alpha", 1.f, 0.f, 1.f, "alpha");
  p_node->add_attr<BoolAttribute>("reverse", false, "reverse");
  p_node->add_attr<BoolAttribute>("clamp", true, "clamp");

  // attribute(s) order
  p_node->set_attr_ordered_key({"alpha", "reverse", "clamp"});
}

void compute_set_alpha_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMapRGBA *p_in = p_node->get_value_ref<hmap::HeightMapRGBA>("texture in");

  if (p_in)
  {
    hmap::HeightMap     *p_alpha = p_node->get_value_ref<hmap::HeightMap>("alpha");
    hmap::HeightMap     *p_noise = p_node->get_value_ref<hmap::HeightMap>("noise");
    hmap::HeightMapRGBA *p_out = p_node->get_value_ref<hmap::HeightMapRGBA>(
        "texture out");

    *p_out = *p_in;

    if (p_alpha)
    {
      hmap::HeightMap alpha_copy = *p_alpha;

      if (p_noise)
        hmap::transform(alpha_copy,
                        *p_noise,
                        [](hmap::Array &x, hmap::Array &y) { x += y; });

      if (GET("clamp", BoolAttribute))
        hmap::transform(alpha_copy, [](hmap::Array &x) { hmap::clamp(x, 0.f, 1.f); });

      if (GET("reverse", BoolAttribute))
        alpha_copy.inverse();

      p_out->set_alpha(alpha_copy);
    }
    else
    {
      float alpha = GET("alpha", FloatAttribute);

      if (GET("reverse", BoolAttribute))
        alpha = 1.f - alpha;

      p_out->set_alpha(alpha);
    }
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
