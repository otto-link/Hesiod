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
  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input"));
  hmap::HeightMap *p_input_mask = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("mask"));
  hmap::HeightMap *p_input_noise = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("noise"));

  // work on a copy of the input
  this->value_out = *p_input_hmap;

  float zmin = this->value_out.min();
  float zmax = this->value_out.max();

  hmap::transform(this->value_out,
                  p_input_mask,
                  p_input_noise,
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
