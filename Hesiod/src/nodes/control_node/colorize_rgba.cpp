/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/cmap.hpp"
#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ColorizeRGBA::ColorizeRGBA(std::string id) : ControlNode(id)
{
  LOG_DEBUG("ColorizeRGBA::ColorizeRGBA()");
  this->node_type = "ColorizeRGBA";
  this->category = category_mapping.at(this->node_type);

  this->attr["colormap"] = NEW_ATTR_MAPENUM(hesiod::get_colormap_mapping());
  this->attr["reverse"] = NEW_ATTR_BOOL(false);
  this->attr["reverse_alpha"] = NEW_ATTR_BOOL(false);
  this->attr["clamp"] = NEW_ATTR_RANGE(false);
  // NB - GUI for this node is produce "manually", see specific render_settings

  this->add_port(
      gnode::Port("color_level", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("alpha",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("RGBA", gnode::direction::out, dtype::dHeightMapRGBA));
  this->update_inner_bindings();
}

void ColorizeRGBA::compute()
{
  LOG_DEBUG("computing ColorizeRGBA node [%s]", this->id.c_str());

  std::vector<std::vector<float>> colormap_colors = hesiod::get_colormap_data(
      GET_ATTR_MAPENUM("colormap"));

  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "color_level");
  hmap::HeightMap *p_alpha = CAST_PORT_REF(hmap::HeightMap, "alpha");

  this->value_out.set_sto(p_hmap->shape, p_hmap->tiling, p_hmap->overlap);

  float cmin = 0.f;
  float cmax = 1.f;

  if (GET_ATTR_REF_RANGE("clamp")->is_activated())
  {
    float hmin = p_hmap->min();
    float hmax = p_hmap->max();

    hmap::Vec2<float> crange = GET_ATTR_RANGE("clamp");

    cmin = (1.f - crange.x) * hmin + crange.x * hmax;
    cmax = (1.f - crange.y) * hmin + crange.y * hmax;
  }

  bool inverse = GET_ATTR_BOOL("reverse");

  // reverse alpha
  hmap::HeightMap alpha_copy;
  hmap::HeightMap* p_alpha_copy = nullptr;

  if (GET_ATTR_BOOL("reverse_alpha") && p_alpha)
    {
      alpha_copy = *p_alpha;
      alpha_copy.inverse();
      p_alpha_copy = &alpha_copy;
    }
  else
    p_alpha_copy = p_alpha;
  
  // colorize
  this->value_out
      .colorize(*p_hmap, cmin, cmax, colormap_colors, p_alpha_copy, inverse);
}

void ColorizeRGBA::update_inner_bindings()
{
  this->set_p_data("RGBA", (void *)&this->value_out);
}

} // namespace hesiod::cnode
