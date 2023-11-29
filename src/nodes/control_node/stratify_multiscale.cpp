/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

StratifyMultiscale::StratifyMultiscale(std::string id) : ControlNode(id)
{
  this->node_type = "StratifyMultiscale";
  this->category = category_mapping.at(this->node_type);

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));

  this->add_port(gnode::Port("mask",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));

  this->add_port(gnode::Port("noise",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));

  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void StratifyMultiscale::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void StratifyMultiscale::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  hmap::HeightMap *p_mask = CAST_PORT_REF(hmap::HeightMap, "mask");
  hmap::HeightMap *p_noise = CAST_PORT_REF(hmap::HeightMap, "noise");

  // work on a copy of the input
  this->value_out = *p_hmap;

  float zmin = this->value_out.min();
  float zmax = this->value_out.max();

  hmap::transform(this->value_out,
                  p_mask,
                  p_noise,
                  [this, &zmin, &zmax](hmap::Array &h_out,
                                       hmap::Array *p_mask_array,
                                       hmap::Array *p_noise_array)
                  {
                    hmap::stratify_multiscale(h_out,
                                              zmin,
                                              zmax,
                                              this->n_strata,
                                              this->strata_noise,
                                              this->gamma_list,
                                              this->gamma_noise,
                                              (uint)this->seed,
                                              p_mask_array,
                                              p_noise_array);
                  });

  this->value_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
