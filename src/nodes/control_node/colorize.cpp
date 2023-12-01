/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/cmap.hpp"
#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Colorize::Colorize(std::string id) : ControlNode(id)
{
  LOG_DEBUG("Colorize::Colorize()");
  this->node_type = "Colorize";
  this->category = category_mapping.at(this->node_type);

  this->attr["colormap"] = NEW_ATTR_MAPENUM(hesiod::get_colormap_mapping());
  this->attr["reverse"] = NEW_ATTR_BOOL(false);
  this->attr["clamp"] = NEW_ATTR_RANGE(false);
  // NB - GUI for this node is produce "manually", see specific render_settings

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("RGB", gnode::direction::out, dtype::dHeightMapRGB));
  this->add_port(gnode::Port("thru", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void Colorize::compute()
{
  LOG_DEBUG("computing Colorize node [%s]", this->id.c_str());

  // for thru port
  this->update_inner_bindings();
  this->update_links();

  std::vector<std::vector<float>> colormap_colors = hesiod::get_colormap_data(
      GET_ATTR_MAPENUM("colormap"));

  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input"));

  this->value_out.set_sto(p_input_hmap->shape,
                          p_input_hmap->tiling,
                          p_input_hmap->overlap);

  float cmin = 0.f;
  float cmax = 1.f;

  if (GET_ATTR_REF_RANGE("clamp")->is_activated())
  {
    float hmin = p_input_hmap->min();
    float hmax = p_input_hmap->max();

    hmap::Vec2<float> crange = GET_ATTR_RANGE("clamp");

    cmin = (1.f - crange.x) * hmin + crange.x * hmax;
    cmax = (1.f - crange.y) * hmin + crange.y * hmax;
  }

  bool inverse = GET_ATTR_BOOL("reverse");

  this->value_out.colorize(*p_input_hmap, cmin, cmax, colormap_colors, inverse);
}

void Colorize::update_inner_bindings()
{
  this->set_p_data("RGB", (void *)&this->value_out);
  this->set_p_data("thru", this->get_p_data("input"));
}

} // namespace hesiod::cnode
