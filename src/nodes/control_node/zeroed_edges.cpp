/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ZeroedEdges::ZeroedEdges(std::string id) : gnode::Node(id)
{
  this->node_type = "ZeroedEdges";
  this->category = category_mapping.at(this->node_type);

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("dr",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));

  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void ZeroedEdges::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void ZeroedEdges::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input"));
  hmap::HeightMap *p_input_dr = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("dr"));

  this->value_out = *p_input_hmap;

  if (!p_input_dr)
    hmap::transform(
        this->value_out,
        [this](hmap::Array &z, hmap::Vec2<float> shift, hmap::Vec2<float> scale)
        { hmap::zeroed_edges(z, this->sigma, nullptr, shift, scale); });

  else
    hmap::transform(this->value_out,
                    *p_input_dr,
                    [this](hmap::Array      &z,
                           hmap::Array      &dr,
                           hmap::Vec2<float> shift,
                           hmap::Vec2<float> scale)
                    { hmap::zeroed_edges(z, this->sigma, &dr, shift, scale); });

  if (remap)
    this->value_out.remap(this->vmin, this->vmax);
}

} // namespace hesiod::cnode
