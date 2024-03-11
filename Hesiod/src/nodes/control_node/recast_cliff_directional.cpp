/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

RecastCliffDirectional::RecastCliffDirectional(std::string id) : ControlNode(id)
{
  LOG_DEBUG("RecastCliffDirectional::RecastCliffDirectional()");
  this->node_type = "RecastCliffDirectional";
  this->category = category_mapping.at(this->node_type);

  this->attr["talus_global"] = NEW_ATTR_FLOAT(1.f, 0.f, 10.f);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.1f, 0.f, 0.5f);
  this->attr["amplitude"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);
  this->attr["angle"] = NEW_ATTR_FLOAT(0.f, -180.f, 180.f);
  this->attr["gain"] = NEW_ATTR_FLOAT(2.f, 0.01f, 10.f);

  this->attr_ordered_key = {"talus_global",
                            "radius",
                            "amplitude",
                            "angle",
                            "gain"};

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("mask",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void RecastCliffDirectional::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void RecastCliffDirectional::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  hmap::HeightMap *p_mask = CAST_PORT_REF(hmap::HeightMap, "mask");

  this->value_out = *p_hmap;

  float talus = GET_ATTR_FLOAT("talus_global") / (float)this->value_out.shape.x;

  int ir = std::max(1,
                    (int)(GET_ATTR_FLOAT("radius") * this->value_out.shape.x));

  hmap::transform(this->value_out,
                  p_mask,
                  [this, &talus, &ir](hmap::Array &z, hmap::Array *p_mask)
                  {
                    hmap::recast_cliff_directional(z,
                                                   talus,
                                                   ir,
                                                   GET_ATTR_FLOAT("amplitude"),
                                                   GET_ATTR_FLOAT("angle"),
                                                   p_mask,
                                                   GET_ATTR_FLOAT("gain"));
                  });

  this->value_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
