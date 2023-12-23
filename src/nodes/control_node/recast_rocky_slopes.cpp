/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

RecastRockySlopes::RecastRockySlopes(std::string id) : ControlNode(id)
{
  LOG_DEBUG("RecastRockySlopes::RecastRockySlopes()");
  this->node_type = "RecastRockySlopes";
  this->category = category_mapping.at(this->node_type);

  this->attr["talus_global"] = NEW_ATTR_FLOAT(2.f, 0.f, 16.f);
  this->attr["ir"] = NEW_ATTR_INT(32, 1, 128);
  this->attr["kw"] = NEW_ATTR_FLOAT(16.f, 0.01f, 128.f);
  this->attr["amplitude"] = NEW_ATTR_FLOAT(0.05f, 0.01f, 1.f);
  this->attr["gamma"] = NEW_ATTR_FLOAT(0.5f, 0.01f, 10.f);
  this->attr["seed"] = NEW_ATTR_SEED();

  this->attr_ordered_key =
      {"talus_global", "ir", "kw", "amplitude", "gamma", "seed"};

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("noise",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(gnode::Port("mask",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void RecastRockySlopes::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void RecastRockySlopes::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  hmap::HeightMap *p_noise = CAST_PORT_REF(hmap::HeightMap, "noise");
  hmap::HeightMap *p_mask = CAST_PORT_REF(hmap::HeightMap, "mask");

  this->value_out = *p_hmap;

  float talus = GET_ATTR_FLOAT("talus_global") / (float)this->value_out.shape.x;

  hmap::transform(
      this->value_out,
      p_noise,
      p_mask,
      [this, &talus](hmap::Array &z, hmap::Array *p_noise, hmap::Array *p_mask)
      {
        hmap::recast_rocky_slopes(z,
                                  talus,
                                  GET_ATTR_INT("ir"),
                                  GET_ATTR_FLOAT("amplitude"),
                                  GET_ATTR_SEED("seed"),
                                  GET_ATTR_FLOAT("kw"),
                                  p_mask,
                                  GET_ATTR_FLOAT("gamma"),
                                  p_noise);
      });

  this->value_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
