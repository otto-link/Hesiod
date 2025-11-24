/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/constants/cmap.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_set_alpha_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture in");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "alpha");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "noise");
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "texture out", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("alpha", "alpha", 1.f, 0.f, 1.f);
  node.add_attr<BoolAttribute>("reverse", "reverse", false);
  node.add_attr<BoolAttribute>("clamp", "clamp", true);

  // attribute(s) order
  node.set_attr_ordered_key({"alpha", "reverse", "clamp"});
}

void compute_set_alpha_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::HeightmapRGBA *p_in = node.get_value_ref<hmap::HeightmapRGBA>("texture in");

  if (p_in)
  {
    hmap::Heightmap     *p_alpha = node.get_value_ref<hmap::Heightmap>("alpha");
    hmap::Heightmap     *p_noise = node.get_value_ref<hmap::Heightmap>("noise");
    hmap::HeightmapRGBA *p_out = node.get_value_ref<hmap::HeightmapRGBA>("texture out");

    *p_out = *p_in;

    if (p_alpha)
    {
      hmap::Heightmap alpha_copy = *p_alpha;

      if (p_noise)
        hmap::transform(alpha_copy,
                        *p_noise,
                        [](hmap::Array &x, hmap::Array &y) { x += y; });

      if (node.get_attr<BoolAttribute>("clamp"))
        hmap::transform(alpha_copy, [](hmap::Array &x) { hmap::clamp(x, 0.f, 1.f); });

      if (node.get_attr<BoolAttribute>("reverse"))
        alpha_copy.inverse();

      p_out->set_alpha(alpha_copy);
    }
    else
    {
      float alpha = node.get_attr<FloatAttribute>("alpha");

      if (node.get_attr<BoolAttribute>("reverse"))
        alpha = 1.f - alpha;

      p_out->set_alpha(alpha);
    }
  }
}

} // namespace hesiod
