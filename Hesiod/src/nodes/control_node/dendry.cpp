/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Dendry::Dendry(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap)
    : ControlNode(id), shape(shape), tiling(tiling), overlap(overlap)
{
  LOG_DEBUG("Dendry::Dendry, id: %s", id.c_str());

  this->node_type = "Dendry";
  this->category = category_mapping.at(this->node_type);

  this->attr["kw"] = NEW_ATTR_WAVENB(); // TODO default to {8.f, 8.f}
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["eps"] = NEW_ATTR_FLOAT(0.2f, 0.f, 1.f);
  this->attr["resolution"] = NEW_ATTR_INT(1, 1, 8);
  this->attr["displacement"] = NEW_ATTR_FLOAT(0.075f, 0.f, 0.2f);
  this->attr["primitives_resolution_steps"] = NEW_ATTR_INT(3, 1, 8);
  this->attr["slope_power"] = NEW_ATTR_FLOAT(1.f, 0.f, 2.f);
  this->attr["noise_amplitude_proportion"] = NEW_ATTR_FLOAT(0.01f, 0.f, 1.f);
  this->attr["remap"] = NEW_ATTR_RANGE();
  this->attr["inverse"] = NEW_ATTR_BOOL(false);

  this->attr_ordered_key = {"kw",
                            "seed",
                            "eps",
                            "slope_power",
                            "resolution",
                            "displacement",
                            "primitives_resolution_steps",
                            "noise_amplitude_proportion",
                            "remap",
                            "inverse"};

  this->value_out.set_sto(shape, tiling, overlap);

  this->add_port(
      gnode::Port("control_function", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("dx",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(gnode::Port("dy",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void Dendry::compute()
{
  LOG_DEBUG("computing Dendry node [%s]", this->id.c_str());

  hmap::HeightMap *p_ctrl = CAST_PORT_REF(hmap::HeightMap, "control_function");
  hmap::HeightMap *p_dx = CAST_PORT_REF(hmap::HeightMap, "dx");
  hmap::HeightMap *p_dy = CAST_PORT_REF(hmap::HeightMap, "dy");

  hmap::Array ctrl_array = p_ctrl->to_array({128, 128});

  hmap::fill(this->value_out,
             p_dx,
             p_dy,
             [this, &ctrl_array](hmap::Vec2<int>   shape,
                                 hmap::Vec2<float> shift,
                                 hmap::Vec2<float> scale,
                                 hmap::Array      *p_noise_x,
                                 hmap::Array      *p_noise_y)
             {
               return hmap::dendry(shape,
                                   GET_ATTR_WAVENB("kw"),
                                   GET_ATTR_SEED("seed"),
                                   ctrl_array,
                                   GET_ATTR_FLOAT("eps"),
                                   GET_ATTR_INT("resolution"),
                                   GET_ATTR_FLOAT("displacement"),
                                   GET_ATTR_INT("primitives_resolution_steps"),
                                   GET_ATTR_FLOAT("slope_power"),
                                   GET_ATTR_FLOAT("noise_amplitude_proportion"),
                                   true, // add noise
                                   0.5f, // overlap
                                   p_noise_x,
                                   p_noise_y,
                                   shift,
                                   scale);
             });

  // hmap::fill(this->value_out,
  //            *p_ctrl,
  //            p_dx,
  //            p_dy,
  //            [this](hmap::Array       ctrl_array,
  //                   hmap::Vec2<int>   shape,
  //                   hmap::Vec2<float> shift,
  //                   hmap::Vec2<float> scale,
  //                   hmap::Array      *p_noise_x,
  //                   hmap::Array      *p_noise_y)
  //            {
  //              return hmap::dendry(shape,
  //                                  GET_ATTR_WAVENB("kw"),
  //                                  GET_ATTR_SEED("seed"),
  //                                  ctrl_array,
  //                                  GET_ATTR_FLOAT("eps"),
  //                                  GET_ATTR_INT("resolution"),
  //                                  GET_ATTR_FLOAT("displacement"),
  //                                  GET_ATTR_INT("primitives_resolution_steps"),
  //                                  GET_ATTR_FLOAT("slope_power"),
  //                                  GET_ATTR_FLOAT("noise_amplitude_proportion"),
  //                                  true, // add noise
  //                                  0.5f, // overlap
  //                                  p_noise_x,
  //                                  p_noise_y,
  //                                  shift,
  //                                  scale);
  //            });

  this->value_out.smooth_overlap_buffers();
  this->post_process_heightmap(this->value_out);
}

void Dendry::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
