/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ExportOBJ::ExportOBJ(std::string id) : ControlNode(id)
{
  LOG_DEBUG("ExportOBJ::ExportOBJ()");
  this->node_type = "ExportOBJ";
  this->category = category_mapping.at(this->node_type);

  this->attr["auto_export"] = NEW_ATTR_BOOL(false);
  this->attr["fname"] = NEW_ATTR_FILENAME("export.obj");
  this->attr["radius"] = NEW_ATTR_FLOAT(0.f, 0.f, 0.2f, "%.3f");
  this->attr["max_error"] = NEW_ATTR_FLOAT(5e-4f, 0.f, 0.01f, "%.4f");
  this->attr["elevation_scaling"] = NEW_ATTR_FLOAT(0.2f, 0.f, 1.f);

  this->attr_ordered_key = {"auto_export",
                            "fname",
                            "radius",
                            "max_error",
                            "elevation_scaling"};

  this->add_port(
      gnode::Port("elevation", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("RGB",
                             gnode::direction::in,
                             dtype::dHeightMapRGB,
                             gnode::optional::yes));
}

void ExportOBJ::compute()
{
  if (GET_ATTR_BOOL("auto_export"))
    this->write_file();
}

void ExportOBJ::write_file()
{
  if (this->get_p_data("elevation"))
  {
    hmap::HeightMap *p_h = (hmap::HeightMap *)this->get_p_data("elevation");
    hmap::Array      array = p_h->to_array();

    std::string fname = GET_ATTR_FILENAME("fname");
    int         ir = (int)(GET_ATTR_FLOAT("radius") * array.shape.x);

    // if available export RGB to an image file
    std::string texture_fname = "";
    if (this->get_p_data("RGB"))
    {
      texture_fname = fname + ".png";
      ((hmap::HeightMapRGB *)this->get_p_data("RGB"))
          ->to_png_16bit(texture_fname);
    }

    // export mesh
    if (ir == 0)
      hmap::export_wavefront_obj(fname,
                                 array,
                                 hmap::MeshType::tri_optimized,
                                 GET_ATTR_FLOAT("elevation_scaling"),
                                 texture_fname,
                                 GET_ATTR_FLOAT("max_error"));
    else
      hmap::export_wavefront_obj(fname,
                                 array,
                                 ir,
                                 GET_ATTR_FLOAT("elevation_scaling"),
                                 texture_fname,
                                 GET_ATTR_FLOAT("max_error"));
  }
}

} // namespace hesiod::cnode
