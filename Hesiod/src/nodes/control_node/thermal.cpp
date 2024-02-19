/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Thermal::Thermal(std::string id) : ControlNode(id)
{
  this->node_type = "Thermal";
  this->category = category_mapping.at(this->node_type);

  this->attr["talus_global"] = NEW_ATTR_FLOAT(0.1f, 0.f, 10.f);
  this->attr["iterations"] = NEW_ATTR_INT(5, 1, 200);

  this->attr_ordered_key = {"talus_global", "iterations"};

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("bedrock",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(gnode::Port("mask",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));

  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->add_port(
      gnode::Port("deposition map", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();

  this->deposition_map = hmap::HeightMap();
}

void Thermal::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());

  this->set_p_data("output", (void *)&(this->value_out));
  this->set_p_data("deposition map", (void *)&(this->deposition_map));
}

void Thermal::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  hmap::HeightMap *p_bedrock = CAST_PORT_REF(hmap::HeightMap, "bedrock");
  hmap::HeightMap *p_mask = CAST_PORT_REF(hmap::HeightMap, "mask");

  hmap::HeightMap *p_deposition = CAST_PORT_REF(hmap::HeightMap,
                                                "deposition map");

  // work on a copy of the input
  this->value_out = *p_hmap;

  if (p_deposition)
    this->deposition_map.set_sto(p_hmap->shape,
                                 p_hmap->tiling,
                                 p_hmap->overlap);

  float talus = GET_ATTR_FLOAT("talus_global") / (float)this->value_out.shape.x;

  hmap::transform(this->value_out,
                  p_bedrock,
                  p_mask,
                  p_deposition,
                  [this, &talus](hmap::Array &h_out,
                                 hmap::Array *p_bedrock_array,
                                 hmap::Array *p_mask_array,
                                 hmap::Array *p_deposition_array)
                  {
                    hmap::thermal(h_out,
                                  p_mask_array,
                                  hmap::constant(h_out.shape, talus),
                                  GET_ATTR_INT("iterations"),
                                  p_bedrock_array,
                                  p_deposition_array);
                  });

  this->value_out.smooth_overlap_buffers();

  if (p_deposition)
    p_deposition->smooth_overlap_buffers();
}

} // namespace hesiod::cnode
