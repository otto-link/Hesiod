/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

StratifyOblique::StratifyOblique(std::string id) : ControlNode(id)
{
  this->node_type = "StratifyOblique";
  this->category = category_mapping.at(this->node_type);

  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["n_strata"] = NEW_ATTR_INT(3, 1, 16);
  this->attr["strata_noise"] = NEW_ATTR_FLOAT(0.f, 0.f, 1.f);
  this->attr["gamma"] = NEW_ATTR_FLOAT(0.7f, 0.01f, 10.f);
  this->attr["gamma_noise"] = NEW_ATTR_FLOAT(0.f, 0.f, 1.f);
  this->attr["talus_global"] = NEW_ATTR_FLOAT(2.f, 0.f, 32.f);
  this->attr["angle"] = NEW_ATTR_FLOAT(30.f, -180.f, 180.f);

  this->attr_ordered_key =
      {"seed", "n_strata", "strata_noise", "gamma", "talus_global", "angle"};

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

void StratifyOblique::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void StratifyOblique::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  hmap::HeightMap *p_noise = CAST_PORT_REF(hmap::HeightMap, "noise");
  hmap::HeightMap *p_mask = CAST_PORT_REF(hmap::HeightMap, "mask");

  // work on a copy of the input
  this->value_out = *p_hmap;

  float zmin = this->value_out.min();
  float zmax = this->value_out.max();

  auto hs = hmap::linspace_jitted(zmin,
                                  zmax,
                                  GET_ATTR_INT("n_strata"),
                                  GET_ATTR_FLOAT("strata_noise"),
                                  GET_ATTR_SEED("seed"));

  float gmin = std::max(0.01f,
                        GET_ATTR_FLOAT("gamma") *
                            (1.f - GET_ATTR_FLOAT("gamma_noise")));
  float gmax = GET_ATTR_FLOAT("gamma") * (1.f + GET_ATTR_FLOAT("gamma_noise"));

  auto gs = hmap::random_vector(gmin,
                                gmax,
                                GET_ATTR_INT("n_strata") - 1,
                                GET_ATTR_SEED("seed"));

  float talus = GET_ATTR_FLOAT("talus_global") / (float)this->value_out.shape.x;

  hmap::transform(this->value_out,
                  p_mask,
                  p_noise,
                  [this, &hs, &gs, &talus](hmap::Array &h_out,
                                           hmap::Array *p_mask_array,
                                           hmap::Array *p_noise_array)
                  {
                    hmap::stratify_oblique(h_out,
                                           p_mask_array,
                                           hs,
                                           gs,
                                           talus,
                                           GET_ATTR_FLOAT("angle"),
                                           p_noise_array);
                  });

  this->value_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
