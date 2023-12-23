/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ZeroedEdges::ZeroedEdges(std::string id) : ControlNode(id)
{
  this->node_type = "ZeroedEdges";
  this->category = category_mapping.at(this->node_type);

  this->attr["sigma"] = NEW_ATTR_FLOAT(0.25f, 0.f, 0.5f);
  this->attr["remap"] = NEW_ATTR_RANGE(false);

  this->attr_ordered_key = {"sigma", "remap"};

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

  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  hmap::HeightMap *p_dr = CAST_PORT_REF(hmap::HeightMap, "dr");

  this->value_out = *p_hmap;

  float sigma = GET_ATTR_FLOAT("sigma");

  if (!p_dr)
    hmap::transform(this->value_out,
                    [this, &sigma](hmap::Array      &z,
                                   hmap::Vec2<float> shift,
                                   hmap::Vec2<float> scale)
                    { hmap::zeroed_edges(z, sigma, nullptr, shift, scale); });

  else
    hmap::transform(this->value_out,
                    *p_dr,
                    [this, &sigma](hmap::Array      &z,
                                   hmap::Array      &dr,
                                   hmap::Vec2<float> shift,
                                   hmap::Vec2<float> scale)
                    { hmap::zeroed_edges(z, sigma, &dr, shift, scale); });

  // remap
  if (GET_ATTR_REF_RANGE("remap")->is_activated())
  {
    hmap::Vec2<float> vrange = GET_ATTR_RANGE("remap");
    this->value_out.remap(vrange.x, vrange.y);
  }
}

} // namespace hesiod::cnode
