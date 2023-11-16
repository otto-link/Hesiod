/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"
#include "hesiod/timer.hpp"

namespace hesiod::cnode
{

FbmSimplex::FbmSimplex(std::string     id,
                       hmap::Vec2<int> shape,
                       hmap::Vec2<int> tiling,
                       float           overlap)
    : Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("FbmSimplex::FbmSimplex()");
  this->node_type = "FbmSimplex";
  this->category = category_mapping.at(this->node_type);

  this->add_port(gnode::Port("stretching",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));

  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void FbmSimplex::compute()
{
  LOG_DEBUG("computing FbmSimplex node [%s]", this->id.c_str());

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
               return hmap::fbm_simplex(shape,
                                        this->kw,
                                        (uint)this->seed,
                                        this->octaves,
                                        this->weight,
                                        this->persistence,
                                        this->lacunarity,
                                        p_noise_x,
                                        p_noise_y,
                                        p_stretching,
                                        shift,
                                        scale);
             });

  // remap the output
  if (this->inverse)
    this->value_out.remap(this->vmax, this->vmin);
  else
    this->value_out.remap(this->vmin, this->vmax);
}

} // namespace hesiod::cnode