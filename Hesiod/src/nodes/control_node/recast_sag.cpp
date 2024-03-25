/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

RecastSag::RecastSag(std::string id) : ControlNode(id)
{
  LOG_DEBUG("RecastSag::RecastSag()");
  this->node_type = "RecastSag";
  this->category = category_mapping.at(this->node_type);

  this->attr["vref"] = NEW_ATTR_FLOAT(0.5f, -1.f, 2.f);
  this->attr["k"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 1.f);

  this->attr_ordered_key = {"vref", "k"};

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  // this->add_port(gnode::Port("mask",
  //                            gnode::direction::in,
  //                            dtype::dHeightMap,
  //                            gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void RecastSag::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void RecastSag::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  // hmap::HeightMap *p_mask = CAST_PORT_REF(hmap::HeightMap, "mask");

  this->value_out = *p_hmap;

  hmap::transform(
      this->value_out,
      nullptr, // p_mask,
      [this](hmap::Array &z, hmap::Array *p_mask)
      { hmap::recast_sag(z, GET_ATTR_FLOAT("vref"), GET_ATTR_FLOAT("k")); });

  this->value_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
