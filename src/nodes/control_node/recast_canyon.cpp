/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

RecastCanyon::RecastCanyon(std::string id) : ControlNode(id)
{
  LOG_DEBUG("RecastCanyon::RecastCanyon()");
  this->node_type = "RecastCanyon";
  this->category = category_mapping.at(this->node_type);

  this->attr["vcut"] = NEW_ATTR_FLOAT(0.7f, -1.f, 2.f);
  this->attr["gamma"] = NEW_ATTR_FLOAT(4.f, 0.01f, 10.f);

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("dz",
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

void RecastCanyon::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void RecastCanyon::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input"));
  hmap::HeightMap *p_input_dz = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("dz"));
  hmap::HeightMap *p_input_mask = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("mask"));

  this->value_out = *p_input_hmap;

  hmap::transform(
      this->value_out,
      p_input_dz,
      p_input_mask,
      [this](hmap::Array &z, hmap::Array *p_noise, hmap::Array *p_mask)
      {
        hmap::recast_canyon(z,
                            GET_ATTR_FLOAT("vcut"),
                            p_mask,
                            GET_ATTR_FLOAT("gamma"),
                            p_noise);
      });

  this->value_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
