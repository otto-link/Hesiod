/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

FbmWorley::FbmWorley(std::string     id,
                     hmap::Vec2<int> shape,
                     hmap::Vec2<int> tiling,
                     float           overlap)
    : Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("FbmWorley::FbmWorley()");
  this->node_type = "FbmWorley";
  this->category = category_mapping.at(this->node_type);

    this->add_port(gnode::Port("stretching",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
    
  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void FbmWorley::compute()
{
  LOG_DEBUG("computing FbmWorley node [%s]", this->id.c_str());

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
               return hmap::fbm_worley(shape,
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
  this->value_out.remap(this->vmin, this->vmax);
}

} // namespace hesiod::cnode
