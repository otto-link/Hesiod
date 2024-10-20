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

void setup_colorize_cmap_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "level");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "alpha");
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::OUT, "texture", CONFIG);

  // attribute(s)
  p_node->add_attr<MapEnumAttribute>("colormap", get_colormap_mapping(), "colormap");
  p_node->add_attr<BoolAttribute>("reverse_colormap", false, "reverse_colormap");
  p_node->add_attr<BoolAttribute>("reverse_alpha", false, "reverse_alpha");
  p_node->add_attr<BoolAttribute>("clamp_alpha", true, "clamp_alpha");
  p_node->add_attr<RangeAttribute>("saturate_input", "saturate_input", false);
  p_node->add_attr<RangeAttribute>("saturate_alpha", "saturate_alpha", false);

  // attribute(s) order
  p_node->set_attr_ordered_key({"colormap",
                                "reverse_colormap",
                                "reverse_alpha",
                                "clamp_alpha",
                                "_SEPARATOR_",
                                "saturate_input",
                                "saturate_alpha"});
}

void compute_colorize_cmap_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_level = p_node->get_value_ref<hmap::HeightMap>("level");

  // colorize
  if (p_level)
  {
    hmap::HeightMap     *p_alpha = p_node->get_value_ref<hmap::HeightMap>("alpha");
    hmap::HeightMapRGBA *p_out = p_node->get_value_ref<hmap::HeightMapRGBA>("texture");

    std::vector<std::vector<float>> colormap_colors = hesiod::get_colormap_data(
        GET("colormap", MapEnumAttribute));

    // input saturation (clamping and then remapping to [0, 1])
    float cmin = 0.f;
    float cmax = 1.f;

    if (GET_ATTR("saturate_input", RangeAttribute, is_active))
    {
      float hmin = p_level->min();
      float hmax = p_level->max();

      hmap::Vec2<float> crange = GET("saturate_input", RangeAttribute);

      cmin = (1.f - crange.x) * hmin + crange.x * hmax;
      cmax = (1.f - crange.y) * hmin + crange.y * hmax;
    }

    // reverse alpha
    hmap::HeightMap  alpha_copy;
    hmap::HeightMap *p_alpha_copy = nullptr;

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

      if (GET_ATTR("saturate_alpha", RangeAttribute, is_active))
      {
        hmap::Vec2<float> arange = GET("saturate_alpha", RangeAttribute);
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
                    GET("reverse_colormap", BoolAttribute));
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
