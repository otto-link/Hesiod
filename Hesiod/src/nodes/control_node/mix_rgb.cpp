/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

MixRGB::MixRGB(std::string id) : ControlNode(id)
{
  LOG_DEBUG("MixRGB::MixRGB()");
  this->node_type = "MixRGB";
  this->category = category_mapping.at(this->node_type);

  this->attr["t"] = NEW_ATTR_FLOAT(0.5f, 0.f, 1.f);
  this->attr["sqrt_mix"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key = {"t", "sqrt_mix"};

  this->add_port(
      gnode::Port("RGB1", gnode::direction::in, dtype::dHeightMapRGB));
  this->add_port(
      gnode::Port("RGB2", gnode::direction::in, dtype::dHeightMapRGB));
  this->add_port(gnode::Port("t",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("RGB", gnode::direction::out, dtype::dHeightMapRGB));
  this->update_inner_bindings();
}

void MixRGB::compute()
{
  LOG_DEBUG("computing MixRGB node [%s]", this->id.c_str());

  hmap::HeightMapRGB *p_rgb1 = CAST_PORT_REF(hmap::HeightMapRGB, "RGB1");
  hmap::HeightMapRGB *p_rgb2 = CAST_PORT_REF(hmap::HeightMapRGB, "RGB2");

  if (GET_ATTR_BOOL("sqrt_mix"))
  {
    if (this->get_p_data("t"))
    {
      hmap::HeightMap *p_t = CAST_PORT_REF(hmap::HeightMap, "t");

      // clamp the mixing parameter to avoid artifacts
      hmap::HeightMap tc = *p_t;
      hmap::transform(tc, [](hmap::Array &x) { hmap::clamp(x, 0.f, 1.f); });

      this->value_out = mix_heightmap_rgb_sqrt(*p_rgb1, *p_rgb2, tc);
    }
    else
      this->value_out = mix_heightmap_rgb_sqrt(*p_rgb1,
                                               *p_rgb2,
                                               GET_ATTR_FLOAT("t"));
  }
  else
  {
    // linear salpha mix
    if (this->get_p_data("t"))
    {
      hmap::HeightMap *p_t = CAST_PORT_REF(hmap::HeightMap, "t");

      hmap::HeightMap tc = *p_t;
      hmap::transform(tc, [](hmap::Array &x) { hmap::clamp(x, 0.f, 1.f); });

      this->value_out = mix_heightmap_rgb(*p_rgb1, *p_rgb2, tc);
    }
    else
      this->value_out = mix_heightmap_rgb(*p_rgb1,
                                          *p_rgb2,
                                          GET_ATTR_FLOAT("t"));
  }
}

void MixRGB::update_inner_bindings()
{
  this->set_p_data("RGB", (void *)&this->value_out);
}

} // namespace hesiod::cnode
