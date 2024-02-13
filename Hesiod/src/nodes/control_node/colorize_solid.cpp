/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/cmap.hpp"
#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ColorizeSolid::ColorizeSolid(std::string id) : ControlNode(id)
{
  LOG_DEBUG("ColorizeSolid::ColorizeSolid()");
  this->node_type = "ColorizeSolid";
  this->category = category_mapping.at(this->node_type);

  this->attr["color"] = NEW_ATTR_COLOR();

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("RGB", gnode::direction::out, dtype::dHeightMapRGB));
  this->add_port(gnode::Port("thru", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void ColorizeSolid::compute()
{
  LOG_DEBUG("computing ColorizeSolid node [%s]", this->id.c_str());

  // for thru port
  this->update_inner_bindings();
  this->update_links();

  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");

  this->value_out.set_sto(p_hmap->shape, p_hmap->tiling, p_hmap->overlap);

  // actually use a "one color colormap"
  // TODO optimize memeory usage
  std::vector<float>              col3 = GET_ATTR_COLOR("color");
  std::vector<std::vector<float>> colormap_colors = {col3};

  this->value_out.colorize(*p_hmap, 0.f, 1.f, colormap_colors, false);
}

void ColorizeSolid::update_inner_bindings()
{
  this->set_p_data("RGB", (void *)&this->value_out);
  this->set_p_data("thru", this->get_p_data("input"));
}

} // namespace hesiod::cnode
