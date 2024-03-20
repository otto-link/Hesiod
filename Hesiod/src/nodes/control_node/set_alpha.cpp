/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SetAlpha::SetAlpha(std::string id) : ControlNode(id)
{
  this->node_type = "SetAlpha";
  this->category = category_mapping.at(this->node_type);

  this->attr["alpha"] = NEW_ATTR_FLOAT(0.5f, 0.f, 1.f);

  this->add_port(gnode::Port("alpha",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("RGBA", gnode::direction::in, dtype::dHeightMapRGBA));
  this->add_port(
      gnode::Port("RGBA out", gnode::direction::out, dtype::dHeightMapRGBA));

  this->update_inner_bindings();
}

void SetAlpha::update_inner_bindings()
{
  this->set_p_data("RGBA out", (void *)&this->value_out);
}

void SetAlpha::compute()
{
  hmap::HeightMap     *p_alpha = CAST_PORT_REF(hmap::HeightMap, "alpha");
  hmap::HeightMapRGBA *p_rgba = CAST_PORT_REF(hmap::HeightMapRGBA, "RGBA");

  this->value_out = *p_rgba;

  if (p_alpha)
    this->value_out.set_alpha(*p_alpha);
  else
    this->value_out.set_alpha(GET_ATTR_FLOAT("alpha"));
}

} // namespace hesiod::cnode
