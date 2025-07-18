/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/cmap.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_colorize_gradient_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "level");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "alpha");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "noise");
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "texture", CONFIG);

  // attribute(s)
  ADD_ATTR(ColorGradientAttribute, "gradient");
  ADD_ATTR(BoolAttribute, "reverse_colormap", false);
  ADD_ATTR(BoolAttribute, "reverse_alpha", false);
  ADD_ATTR(BoolAttribute, "clamp_alpha", true);

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"gradient", "reverse_colormap", "reverse_alpha", "clamp_alpha"});
}

void compute_colorize_gradient_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_level = p_node->get_value_ref<hmap::Heightmap>("level");

  if (p_level)
  {
    hmap::Heightmap     *p_alpha = p_node->get_value_ref<hmap::Heightmap>("alpha");
    hmap::Heightmap     *p_noise = p_node->get_value_ref<hmap::Heightmap>("noise");
    hmap::HeightmapRGBA *p_out = p_node->get_value_ref<hmap::HeightmapRGBA>("texture");

    // define colormap based on color gradient
    std::vector<std::vector<float>> gradient = GET("gradient", ColorGradientAttribute);
    std::vector<std::vector<float>> colormap_colors = {};

    for (auto &data : gradient)
      colormap_colors.push_back({data[1], data[2], data[3]});

    LOG->trace("ncolors: {}", colormap_colors.size());

    // reverse alpha
    hmap::Heightmap  alpha_copy;
    hmap::Heightmap *p_alpha_copy = nullptr;

    if (!p_alpha)
      p_alpha_copy = p_alpha;
    else
    {
      alpha_copy = *p_alpha;
      p_alpha_copy = &alpha_copy;

      if (GET("clamp_alpha", BoolAttribute))
        hmap::transform(alpha_copy, [](hmap::Array &x) { hmap::clamp(x, 0.f, 1.f); });

      if (GET("reverse_alpha", BoolAttribute))
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
                    colormap_colors,
                    p_alpha_copy,
                    GET("reverse_colormap", BoolAttribute),
                    p_noise);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
