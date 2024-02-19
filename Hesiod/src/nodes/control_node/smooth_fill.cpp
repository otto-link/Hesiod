/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SmoothFill::SmoothFill(std::string id) : ControlNode(id)
{
  this->node_type = "SmoothFill";
  this->category = category_mapping.at(this->node_type);

  this->attr["ir"] = NEW_ATTR_INT(8, 1, 128);
  this->attr["k"] = NEW_ATTR_FLOAT(0.01f, 0.f, 1.f);

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("mask",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->add_port(
      gnode::Port("deposition map", gnode::direction::out, dtype::dHeightMap));

  this->update_inner_bindings();
}

void SmoothFill::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
  this->set_p_data("deposition map", (void *)&(this->deposition_map));
}

void SmoothFill::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  hmap::HeightMap *p_mask = CAST_PORT_REF(hmap::HeightMap, "mask");
  hmap::HeightMap *p_deposition_map = CAST_PORT_REF(hmap::HeightMap,
                                                    "deposition map");

  // work on a copy of the input
  this->value_out = *p_hmap;

  if (p_deposition_map)
    this->deposition_map.set_sto(p_hmap->shape,
                                 p_hmap->tiling,
                                 p_hmap->overlap);

  hmap::transform(
      this->value_out,
      p_mask,
      p_deposition_map,
      [this](hmap::Array &x, hmap::Array *p_mask, hmap::Array *p_deposition)
      {
        hmap::smooth_fill(x,
                          GET_ATTR_INT("ir"),
                          p_mask,
                          GET_ATTR_FLOAT("k"),
                          p_deposition);
      });

  this->value_out.smooth_overlap_buffers();

  if (p_deposition_map)
    (*p_deposition_map).smooth_overlap_buffers();
}

} // namespace hesiod::cnode
