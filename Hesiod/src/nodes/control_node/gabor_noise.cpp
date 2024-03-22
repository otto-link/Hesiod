/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/primitives.hpp"
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

GaborNoise::GaborNoise(std::string     id,
                       hmap::Vec2<int> shape,
                       hmap::Vec2<int> tiling,
                       float           overlap)
    : ControlNode(id), Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("GaborNoise::GaborNoise()");
  this->node_type = "GaborNoise";
  this->category = category_mapping.at(this->node_type);

  this->attr["kw"] = NEW_ATTR_FLOAT(1.f, 0.01f, 64.f);
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["angle"] = NEW_ATTR_FLOAT(0.f, -180.f, 180.f);
  this->attr["width"] = NEW_ATTR_INT(128, 1, 256);
  this->attr["density"] = NEW_ATTR_FLOAT(0.05f, 0.f, 1.f);

  this->attr_ordered_key = {"kw", "seed", "angle", "width", "density"};

  this->remove_port("dx");
  this->remove_port("dy");
  this->update_inner_bindings();
}

void GaborNoise::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

void GaborNoise::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  float density_per_tile = GET_ATTR_FLOAT("density") /
                           (float)this->value_out.get_ntiles();
  int seed = GET_ATTR_SEED("seed");

  hmap::fill(this->value_out,
             [this, &density_per_tile, &seed](hmap::Vec2<int> shape)
             {
               return hmap::gabor_noise(shape,
                                        GET_ATTR_FLOAT("kw"),
                                        GET_ATTR_FLOAT("angle"),
                                        GET_ATTR_INT(">width"),
                                        density_per_tile,
                                        (uint)seed++);
             });

  this->value_out.smooth_overlap_buffers();
  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
