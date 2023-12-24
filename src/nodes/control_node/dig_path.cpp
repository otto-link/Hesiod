/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

DigPath::DigPath(std::string id) : ControlNode(id)
{
  LOG_DEBUG("DigPath::DigPath()");
  this->node_type = "DigPath";
  this->category = category_mapping.at(this->node_type);

  this->attr["width"] = NEW_ATTR_INT(1, 1, 32);
  this->attr["decay"] = NEW_ATTR_INT(2, 1, 32);
  this->attr["flattening_radius"] = NEW_ATTR_INT(16, 1, 32);
  this->attr["force_downhill"] = NEW_ATTR_BOOL(false);
  this->attr["depth"] = NEW_ATTR_FLOAT(0.f, -0.2f, 0.2f);

  this->attr_ordered_key = {"width",
                            "decay",
                            "flattening_radius",
                            "depth",
                            "force_downhill"};

  this->add_port(gnode::Port("path", gnode::direction::in, dtype::dPath));
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void DigPath::compute()
{
  LOG_DEBUG("computing DigPath node [%s]", this->id.c_str());

  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  hmap::Path      *p_path = CAST_PORT_REF(hmap::Path, "path");

  if (p_path->get_npoints() > 1)
  {
    // work on a copy of the input
    this->value_out = *p_hmap;

    if (!GET_ATTR_BOOL("force_downhill"))
    {
      hmap::transform(this->value_out,
                      [this, p_path](hmap::Array &z, hmap::Vec4<float> bbox)
                      {
                        hmap::dig_path(z,
                                       *p_path,
                                       GET_ATTR_INT("width"),
                                       GET_ATTR_INT("decay"),
                                       GET_ATTR_INT("flattening_radius"),
                                       GET_ATTR_BOOL("force_downhill"),
                                       bbox,
                                       GET_ATTR_FLOAT("depth"));
                      });
    }
    else
    {
      // TODO if downhill is activated, so far not distributed
      hmap::Array z_array = this->value_out.to_array();

      hmap::dig_path(z_array,
                     *p_path,
                     GET_ATTR_INT("width"),
                     GET_ATTR_INT("decay"),
                     GET_ATTR_INT("flattening_radius"),
                     GET_ATTR_BOOL("force_downhill"),
                     hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f), // bbox
                     GET_ATTR_FLOAT("depth"));

      this->value_out.from_array_interp(z_array);
    }

    this->value_out.smooth_overlap_buffers();
  }
}

void DigPath::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
