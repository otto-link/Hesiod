/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

RecastPeak::RecastPeak(std::string id) : ControlNode(id)
{
  LOG_DEBUG("RecastPeak::RecastPeak()");
  this->node_type = "RecastPeak";
  this->category = category_mapping.at(this->node_type);

  this->attr["ir"] = NEW_ATTR_INT(32, 1, 128);
  this->attr["gamma"] = NEW_ATTR_FLOAT(2.f, 0.01f, 10.f);
  this->attr["k"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 1.f);

  this->attr_ordered_key = {"ir", "gamma", "k"};

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("mask",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void RecastPeak::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void RecastPeak::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  hmap::HeightMap *p_mask = CAST_PORT_REF(hmap::HeightMap, "mask");

  this->value_out = *p_hmap;

  hmap::transform(this->value_out,
                  p_mask,
                  [this](hmap::Array &z, hmap::Array *p_mask)
                  {
                    hmap::recast_peak(z,
                                      GET_ATTR_INT("ir"),
                                      p_mask,
                                      GET_ATTR_FLOAT("gamma"),
                                      GET_ATTR_FLOAT("k"));
                  });

  this->value_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
