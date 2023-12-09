/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

AbsSmooth::AbsSmooth(std::string id) : ControlNode(id)
{
  LOG_DEBUG("AbsSmooth::AbsSmooth()");
  this->node_type = "Abs";
  this->category = category_mapping.at(this->node_type);

  this->attr["vshift"] = NEW_ATTR_FLOAT(0.f, -1.f, 1.f);
  this->attr["mu"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 1.f);

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("shift",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void AbsSmooth::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void AbsSmooth::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  hmap::HeightMap *p_vshift = CAST_PORT_REF(hmap::HeightMap, "shift");

  this->value_out.set_sto(p_hmap->shape, p_hmap->tiling, p_hmap->overlap);

  if (!p_vshift)
  {
    if (GET_ATTR_FLOAT("vshift") == 0.f)
      hmap::transform(this->value_out,
                      *p_hmap,
                      [this](hmap::Array &out, hmap::Array &in)
                      { out = hmap::abs_smooth(in, GET_ATTR_FLOAT("mu")); });
    else
      hmap::transform(this->value_out,
                      *p_hmap,
                      [this](hmap::Array &out, hmap::Array &in)
                      {
                        out = hmap::abs_smooth(in,
                                               GET_ATTR_FLOAT("mu"),
                                               GET_ATTR_FLOAT("vshift"));
                      });
  }
  else
    hmap::transform(this->value_out,
                    *p_vshift,
                    *p_hmap,
                    [this](hmap::Array &out, hmap::Array &in, hmap::Array &v)
                    { out = hmap::abs_smooth(in, GET_ATTR_FLOAT("mu"), v); });
}

} // namespace hesiod::cnode
