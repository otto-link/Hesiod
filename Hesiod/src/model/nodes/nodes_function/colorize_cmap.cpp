/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/constants/cmap.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_colorize_cmap_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "level");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "alpha");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "noise");
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "texture", CONFIG);

  // attribute(s)
  ADD_ATTR(node,
           EnumAttribute,
           "colormap",
           hesiod::CmapManager::get_instance().get_colormap_name_mapping());
  ADD_ATTR(node, BoolAttribute, "reverse_colormap", false);
  ADD_ATTR(node, BoolAttribute, "reverse_alpha", false);
  ADD_ATTR(node, BoolAttribute, "clamp_alpha", true);
  ADD_ATTR(node, RangeAttribute, "saturate_input", false);
  ADD_ATTR(node, RangeAttribute, "saturate_alpha", false);

  // attribute(s) order
  node.set_attr_ordered_key({"colormap",
                             "reverse_colormap",
                             "reverse_alpha",
                             "clamp_alpha",
                             "_SEPARATOR_",
                             "saturate_input",
                             "saturate_alpha"});

  add_deprecated_warning_label(node, "Use ColorGradient node.");
}

void compute_colorize_cmap_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_level = node.get_value_ref<hmap::Heightmap>("level");

  // colorize
  if (p_level)
  {
    hmap::Heightmap     *p_alpha = node.get_value_ref<hmap::Heightmap>("alpha");
    hmap::Heightmap     *p_noise = node.get_value_ref<hmap::Heightmap>("noise");
    hmap::HeightmapRGBA *p_out = node.get_value_ref<hmap::HeightmapRGBA>("texture");

    std::vector<std::vector<float>>
        colormap_colors = hesiod::CmapManager::get_instance().get_colormap_data(
            GET(node, "colormap", EnumAttribute));

    // input saturation (clamping and then remapping to [0, 1])
    float cmin = 0.f;
    float cmax = 1.f;

    if (GET_MEMBER(node, "saturate_input", RangeAttribute, is_active))
    {
      float hmin = p_level->min();
      float hmax = p_level->max();

      hmap::Vec2<float> crange = GET(node, "saturate_input", RangeAttribute);

      cmin = (1.f - crange.x) * hmin + crange.x * hmax;
      cmax = (1.f - crange.y) * hmin + crange.y * hmax;
    }

    if (p_noise)
    {
      cmin += p_noise->min();
      cmax += p_noise->max();
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

      if (GET(node, "clamp_alpha", BoolAttribute))
        hmap::transform(alpha_copy, [](hmap::Array &x) { hmap::clamp(x, 0.f, 1.f); });

      if (GET(node, "reverse_alpha", BoolAttribute))
        alpha_copy.inverse();

      if (GET_MEMBER(node, "saturate_alpha", RangeAttribute, is_active))
      {
        hmap::Vec2<float> arange = GET(node, "saturate_alpha", RangeAttribute);
        hmap::transform(alpha_copy,
                        [&arange](hmap::Array &x)
                        { hmap::clamp(x, arange.x, arange.y); });
        alpha_copy.remap();
      }
    }

    // colorize
    p_out->colorize(*p_level,
                    cmin,
                    cmax,
                    colormap_colors,
                    p_alpha_copy,
                    GET(node, "reverse_colormap", BoolAttribute),
                    p_noise);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
