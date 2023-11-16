/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/cmap.hpp"
#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Colorize::Colorize(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("Colorize::Colorize()");
  this->node_type = "Colorize";
  this->category = category_mapping.at(this->node_type);
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("RGB", gnode::direction::out, dtype::dHeightMapRGB));
  this->add_port(gnode::Port("thru", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();

  this->cmap_map = hesiod::get_colormap_mapping();
}

void Colorize::compute()
{
  LOG_DEBUG("computing Colorize node [%s]", this->id.c_str());

  // for thru port
  this->update_inner_bindings();
  this->update_links();

  std::vector<std::vector<float>> colormap_colors = hesiod::get_colormap_data(
      this->cmap_choice);

  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input"));

  this->value_out.set_sto(p_input_hmap->shape,
                          p_input_hmap->tiling,
                          p_input_hmap->overlap);

  float cmin = 0.f;
  float cmax = 1.f;

  if (this->clamp)
  {
    float hmin = p_input_hmap->min();
    float hmax = p_input_hmap->max();

    cmin = (1.f - this->vmin) * hmin + this->vmin * hmax;
    cmax = (1.f - this->vmax) * hmin + this->vmax * hmax;
  }

  this->value_out.colorize(*p_input_hmap,
                           cmin,
                           cmax,
                           colormap_colors,
                           this->reverse);
}

void Colorize::update_inner_bindings()
{
  this->set_p_data("RGB", (void *)&this->value_out);
  this->set_p_data("thru", this->get_p_data("input"));
}

} // namespace hesiod::cnode