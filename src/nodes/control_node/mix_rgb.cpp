/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

MixRGB::MixRGB(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("MixRGB::MixRGB()");
  this->node_type = "MixRGB";
  this->category = category_mapping.at(this->node_type);
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

  hmap::HeightMapRGB *p_input_rgb1 = static_cast<hmap::HeightMapRGB *>(
      (void *)this->get_p_data("RGB1"));
  hmap::HeightMapRGB *p_input_rgb2 = static_cast<hmap::HeightMapRGB *>(
      (void *)this->get_p_data("RGB2"));

  if (this->get_p_data("t"))
  {
    hmap::HeightMap *p_input_t = static_cast<hmap::HeightMap *>(
        (void *)this->get_p_data("t"));
    this->value_out = mix_heightmap_rgb_ryb(*p_input_rgb1,
                                            *p_input_rgb2,
                                            *p_input_t);
  }
  else
    this->value_out = mix_heightmap_rgb_srqt(*p_input_rgb1,
                                             *p_input_rgb2,
                                             this->t);
}

void MixRGB::update_inner_bindings()
{
  this->set_p_data("RGB", (void *)&this->value_out);
}

} // namespace hesiod::cnode
