/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/primitives.hpp"
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

NoiseJordan::NoiseJordan(std::string     id,
                         hmap::Vec2<int> shape,
                         hmap::Vec2<int> tiling,
                         float           overlap)
    : ControlNode(id), Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("NoiseJordan::NoiseJordan()");
  this->node_type = "NoiseJordan";
  this->category = category_mapping.at(this->node_type);

  this->attr["noise_type"] = NEW_ATTR_MAPENUM(this->noise_type_map);
  this->attr["kw"] = NEW_ATTR_WAVENB();
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["octaves"] = NEW_ATTR_INT(8, 0, 32);
  this->attr["weight"] = NEW_ATTR_FLOAT(0.7f, 0.f, 1.f);
  this->attr["persistence"] = NEW_ATTR_FLOAT(0.5f, 0.f, 1.f);
  this->attr["lacunarity"] = NEW_ATTR_FLOAT(2.f, 0.01f, 4.f);
  this->attr["warp0"] = NEW_ATTR_FLOAT(0.2f, 0.f, 1.f);
  this->attr["damp0"] = NEW_ATTR_FLOAT(1.f, 0.f, 1.f);
  this->attr["warp_scale"] = NEW_ATTR_FLOAT(0.2f, 0.f, 1.f);
  this->attr["damp_scale"] = NEW_ATTR_FLOAT(1.f, 0.f, 1.f);

  this->attr_ordered_key = {"noise_type",
                            "kw",
                            "seed",
                            "octaves",
                            "weight",
                            "persistence",
                            "lacunarity",
                            "warp0",
                            "damp0",
                            "warp_scale",
                            "damp_scale",
                            "inverse",
                            "remap"};

  this->add_port(gnode::Port("base_elevation",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
}

void NoiseJordan::compute()
{
  LOG_DEBUG("computing NoiseJordan node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("base_elevation"),
             (hmap::HeightMap *)this->get_p_data("dx"),
             (hmap::HeightMap *)this->get_p_data("dy"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec4<float> bbox,
                    hmap::Array      *p_base_elevation,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y)
             {
               return hmap::noise_jordan(
                   (hmap::NoiseType)GET_ATTR_MAPENUM("noise_type"),
                   shape,
                   GET_ATTR_WAVENB("kw"),
                   GET_ATTR_SEED("seed"),
                   GET_ATTR_INT("octaves"),
                   GET_ATTR_FLOAT("weight"),
                   GET_ATTR_FLOAT("persistence"),
                   GET_ATTR_FLOAT("lacunarity"),
                   GET_ATTR_FLOAT("warp0"),
                   GET_ATTR_FLOAT("damp0"),
                   GET_ATTR_FLOAT("warp_scale"),
                   GET_ATTR_FLOAT("damp_scale"),
                   p_base_elevation,
                   p_noise_x,
                   p_noise_y,
                   nullptr,
                   bbox);
             });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
