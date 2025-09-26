/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/cmap.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_set_alpha_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture in");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "alpha");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "noise");
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "texture out", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "alpha", 1.f, 0.f, 1.f);
  ADD_ATTR(BoolAttribute, "reverse", false);
  ADD_ATTR(BoolAttribute, "clamp", true);

  // attribute(s) order
  p_node->set_attr_ordered_key({"alpha", "reverse", "clamp"});
}

void compute_set_alpha_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::HeightmapRGBA *p_in = p_node->get_value_ref<hmap::HeightmapRGBA>("texture in");

  if (p_in)
  {
    hmap::Heightmap     *p_alpha = p_node->get_value_ref<hmap::Heightmap>("alpha");
    hmap::Heightmap     *p_noise = p_node->get_value_ref<hmap::Heightmap>("noise");
    hmap::HeightmapRGBA *p_out = p_node->get_value_ref<hmap::HeightmapRGBA>(
        "texture out");

    *p_out = *p_in;

    if (p_alpha)
    {
      hmap::Heightmap alpha_copy = *p_alpha;

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
