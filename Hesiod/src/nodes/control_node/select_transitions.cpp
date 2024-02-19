/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SelectTransitions::SelectTransitions(std::string id) : ControlNode(id)
{
  this->node_type = "SelectTransitions";
  this->category = category_mapping.at(this->node_type);

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing"] = NEW_ATTR_BOOL(false);
  this->attr["ir_smoothing"] = NEW_ATTR_INT(4, 1, 128);
  this->attr["saturate"] = NEW_ATTR_RANGE(false);
  this->attr["k_saturate"] = NEW_ATTR_FLOAT(0.05f, 0.f, 1.f);
  this->attr["remap"] = NEW_ATTR_RANGE(true);

  this->attr_ordered_key = {"inverse",
                            "smoothing",
                            "_ir_smoothing",
                            "saturate",
                            "_k_saturate",
                            "remap"};

  this->add_port(
      gnode::Port("input##1", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("input##2", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("blend", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void SelectTransitions::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void SelectTransitions::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_hmap1 = CAST_PORT_REF(hmap::HeightMap, "input##1");
  hmap::HeightMap *p_hmap2 = CAST_PORT_REF(hmap::HeightMap, "input##2");
  hmap::HeightMap *p_blend = CAST_PORT_REF(hmap::HeightMap, "blend");

  this->value_out.set_sto(p_hmap1->shape, p_hmap1->tiling, p_hmap1->overlap);

  hmap::transform(
      this->value_out,
      *p_hmap1,
      *p_hmap2,
      *p_blend,
      [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2, hmap::Array &a3)
      { m = hmap::select_transitions(a1, a2, a3); });
}

} // namespace hesiod::cnode
