/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

RidgedPerlin::RidgedPerlin(std::string     id,
                           hmap::Vec2<int> shape,
                           hmap::Vec2<int> tiling,
                           float           overlap)
    : Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("RidgedPerlin::RidgedPerlin()");
  this->node_type = "RidgedPerlin";
  this->category = category_mapping.at(this->node_type);

  this->attr["kw"] = NEW_ATTR_WAVENB();
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["octaves"] = NEW_ATTR_INT(8, 0, 32);
  this->attr["weight"] = NEW_ATTR_FLOAT(0.7f, 0.f, 1.f);
  this->attr["persistence"] = NEW_ATTR_FLOAT(0.5f, 0.f, 1.f);
  this->attr["lacunarity"] = NEW_ATTR_FLOAT(0.5f, 0.01f, 4.f);

  this->attr_ordered_key = {"kw",
                            "seed",
                            "octaves",
                            "weight",
                            "persistence",
                            "lacunarity",
                            "inverse",
                            "remap"};

  this->add_port(gnode::Port("stretching",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->update_inner_bindings();
}

void RidgedPerlin::compute()
{
  LOG_DEBUG("computing RidgedPerlin node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             (hmap::HeightMap *)this->get_p_data("dy"),
             (hmap::HeightMap *)this->get_p_data("stretching"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec2<float> shift,
                    hmap::Vec2<float> scale,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y,
                    hmap::Array      *p_stretching)
             {
               return hmap::ridged_perlin(shape,
                                          GET_ATTR_WAVENB("kw"),
                                          GET_ATTR_SEED("seed"),
                                          GET_ATTR_INT("octaves"),
                                          GET_ATTR_FLOAT("weight"),
                                          GET_ATTR_FLOAT("persistence"),
                                          GET_ATTR_FLOAT("lacunarity"),
                                          p_noise_x,
                                          p_noise_y,
                                          p_stretching,
                                          shift,
                                          scale);
             });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
