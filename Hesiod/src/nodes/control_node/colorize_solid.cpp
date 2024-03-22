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
      gnode::Port("RGBA", gnode::direction::out, dtype::dHeightMapRGBA));
  this->add_port(gnode::Port("thru", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void ColorizeSolid::compute()
{
  LOG_DEBUG("computing ColorizeSolid node [%s]", this->id.c_str());

  // for thru port
  this->update_inner_bindings();
  this->update_links();

  hmap::HeightMap   *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  std::vector<float> col3 = GET_ATTR_COLOR("color");

  this->value_out.set_sto(p_hmap->shape, p_hmap->tiling, p_hmap->overlap);

  for (int k = 0; k < 4; k++)
  {
    float color = k < 3 ? col3[k] : 1.f;
    LOG_DEBUG("%f", color);
    this->value_out.rgba[k] =
        hmap::HeightMap(p_hmap->shape, p_hmap->tiling, p_hmap->overlap, color);
  }
}

void ColorizeSolid::update_inner_bindings()
{
  this->set_p_data("RGBA", (void *)&this->value_out);
  this->set_p_data("thru", this->get_p_data("input"));
}

} // namespace hesiod::cnode
