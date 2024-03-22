/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

MixRGBA::MixRGBA(std::string id) : ControlNode(id)
{
  LOG_DEBUG("MixRGBA::MixRGBA()");
  this->node_type = "MixRGBA";
  this->category = category_mapping.at(this->node_type);

  this->attr["use_sqrt_avg"] = NEW_ATTR_BOOL(true);
  this->attr["reset_output_alpha"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key = {"use_sqrt_avg", "reset_output_alpha"};

  this->add_port(
      gnode::Port("RGBA1", gnode::direction::in, dtype::dHeightMapRGBA));
  this->add_port(
      gnode::Port("RGBA2", gnode::direction::in, dtype::dHeightMapRGBA));
  this->add_port(gnode::Port("RGBA3",
                             gnode::direction::in,
                             dtype::dHeightMapRGBA,
                             gnode::optional::yes));
  this->add_port(gnode::Port("RGBA4",
                             gnode::direction::in,
                             dtype::dHeightMapRGBA,
                             gnode::optional::yes));

  this->add_port(
      gnode::Port("RGBA", gnode::direction::out, dtype::dHeightMapRGBA));
  this->update_inner_bindings();
}

void MixRGBA::compute()
{
  LOG_DEBUG("computing MixRGBA node [%s]", this->id.c_str());

  hmap::HeightMapRGBA *p_rgb1 = CAST_PORT_REF(hmap::HeightMapRGBA, "RGBA1");
  hmap::HeightMapRGBA *p_rgb2 = CAST_PORT_REF(hmap::HeightMapRGBA, "RGBA2");
  hmap::HeightMapRGBA *p_rgb3 = CAST_PORT_REF(hmap::HeightMapRGBA, "RGBA3");
  hmap::HeightMapRGBA *p_rgb4 = CAST_PORT_REF(hmap::HeightMapRGBA, "RGBA4");

  std::vector<hmap::HeightMapRGBA *> ptr_list = {};
  for (auto &ptr : {p_rgb1, p_rgb2, p_rgb3, p_rgb4})
    if (ptr)
      ptr_list.push_back(ptr);

  this->value_out = mix_heightmap_rgba(ptr_list, GET_ATTR_BOOL("use_sqrt_avg"));

  if (GET_ATTR_BOOL("reset_output_alpha"))
    this->value_out.set_alpha(1.f);
}

void MixRGBA::update_inner_bindings()
{
  this->set_p_data("RGBA", (void *)&this->value_out);
}

} // namespace hesiod::cnode
