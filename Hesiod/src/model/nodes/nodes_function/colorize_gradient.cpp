/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/heightmap.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/constants/color_gradient.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_colorize_gradient_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "level");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "alpha");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "noise");
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "texture", CONFIG(node));

  // attribute(s)
  node.add_attr<ColorGradientAttribute>("gradient", "gradient");
  node.add_attr<BoolAttribute>("reverse_colormap", "reverse_colormap", false);
  node.add_attr<BoolAttribute>("reverse_alpha", "reverse_alpha", false);
  node.add_attr<BoolAttribute>("clamp_alpha", "clamp_alpha", true);

  // attribute(s) order
  node.set_attr_ordered_key(
      {"gradient", "_SEPARATOR_", "reverse_colormap", "reverse_alpha", "clamp_alpha"});

  // add presets
  node.get_attr_ref<ColorGradientAttribute>("gradient")
      ->set_presets(ColorGradientManager::get_instance().get_as_attr_presets());
}

void compute_colorize_gradient_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_level = node.get_value_ref<hmap::Heightmap>("level");

  if (p_level)
  {
    hmap::Heightmap     *p_alpha = node.get_value_ref<hmap::Heightmap>("alpha");
    hmap::Heightmap     *p_noise = node.get_value_ref<hmap::Heightmap>("noise");
    hmap::HeightmapRGBA *p_out = node.get_value_ref<hmap::HeightmapRGBA>("texture");

    // define colormap based on color gradient
    std::vector<attr::Stop> gradient = node.get_attr<ColorGradientAttribute>("gradient");
    std::vector<float>      positions = {};
    std::vector<std::vector<float>> colormap_colors = {};

    for (auto &data : gradient)
    {
      positions.push_back(data.position);
      colormap_colors.push_back({data.color[0], data.color[1], data.color[2]});
    }

    // reverse alpha
    hmap::Heightmap  alpha_copy;
    hmap::Heightmap *p_alpha_copy = nullptr;

    if (!p_alpha)
      p_alpha_copy = p_alpha;
    else
    {
      alpha_copy = *p_alpha;
      p_alpha_copy = &alpha_copy;

      if (node.get_attr<BoolAttribute>("clamp_alpha"))
        hmap::transform(alpha_copy, [](hmap::Array &x) { hmap::clamp(x, 0.f, 1.f); });

      if (node.get_attr<BoolAttribute>("reverse_alpha"))
        alpha_copy.inverse();
    }

    // colorize
    float cmin = p_level->min();
    float cmax = p_level->max();

    if (p_noise)
    {
      cmin += p_noise->min();
      cmax += p_noise->max();
    }

    p_out->colorize(*p_level,
                    cmin,
                    cmax,
                    positions,
                    colormap_colors,
                    p_alpha_copy,
                    node.get_attr<BoolAttribute>("reverse_colormap"),
                    p_noise);
  }
}

} // namespace hesiod
