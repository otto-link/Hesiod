/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

PathToHeightmapGaussian::PathToHeightmapGaussian(std::string     id,
                                                 hmap::Vec2<int> shape,
                                                 hmap::Vec2<int> tiling,
                                                 float           overlap)
    : ControlNode(id), shape(shape), tiling(tiling), overlap(overlap)
{
  LOG_DEBUG("PathToHeightmapGaussian::PathToHeightmapGaussian()");
  this->node_type = "PathToHeightmapGaussian";
  this->category = category_mapping.at(this->node_type);

  this->attr["width"] = NEW_ATTR_FLOAT(0.25f, 0.01f, 1.f);
  this->attr["remap"] = NEW_ATTR_RANGE();
  this->attr["inverse"] = NEW_ATTR_BOOL(false);

  this->attr_ordered_key = {"width", "inverse", "remap"};

  this->add_port(gnode::Port("path", gnode::direction::in, dtype::dPath));
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
  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void PathToHeightmapGaussian::compute()
{
  LOG_DEBUG("computing PathToHeightmapGaussian node [%s]", this->id.c_str());

  hmap::Path      *p_input_path = CAST_PORT_REF(hmap::Path, "path");
  hmap::HeightMap *p_dx = CAST_PORT_REF(hmap::HeightMap, "dx");
  hmap::HeightMap *p_dy = CAST_PORT_REF(hmap::HeightMap, "dy");

  if (p_input_path->get_npoints() > 1)
  {
    hmap::fill(this->value_out,
               p_dx,
               p_dy,
               [p_input_path, this](hmap::Vec2<int>   shape,
                                    hmap::Vec2<float> shift,
                                    hmap::Vec2<float> scale,
                                    hmap::Array      *p_noise_x,
                                    hmap::Array      *p_noise_y)
               {
                 hmap::Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f};

                 return p_input_path->to_array_gaussian(shape,
                                                        bbox,
                                                        GET_ATTR_FLOAT("width"),
                                                        p_noise_x,
                                                        p_noise_y,
                                                        shift,
                                                        scale);
               });
  }
  else
    // fill with zeros
    hmap::transform(this->value_out, [](hmap::Array x) { x = 0.f; });

  this->post_process_heightmap(this->value_out);
}

void PathToHeightmapGaussian::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
