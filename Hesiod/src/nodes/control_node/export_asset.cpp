/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ExportAsset::ExportAsset(std::string id) : ControlNode(id)
{
  LOG_DEBUG("ExportAsset::ExportAsset()");
  this->node_type = "ExportAsset";
  this->category = category_mapping.at(this->node_type);

  // generate export format map
  for (auto &[export_id, export_infos] : hmap::asset_export_format_as_string)
    this->export_format_map[export_infos[0]] = (int)export_id;

  for (auto &[mesh_id, mesh_infos] : hmap::mesh_type_as_string)
    this->export_mesh_map[mesh_infos] = (int)mesh_id;

  this->attr["auto_export"] = NEW_ATTR_BOOL(false);
  this->attr["fname"] = NEW_ATTR_FILENAME("export");

  this->attr["export_format"] = NEW_ATTR_MAPENUM(this->export_format_map);
  this->attr["mesh_type"] = NEW_ATTR_MAPENUM(this->export_mesh_map);

  this->attr["max_error"] = NEW_ATTR_FLOAT(5e-4f, 0.f, 0.01f, "%.4f");
  this->attr["elevation_scaling"] = NEW_ATTR_FLOAT(0.2f, 0.f, 1.f);

  this->attr_ordered_key = {"auto_export",
                            "fname",
                            "export_format",
                            "mesh_type",
                            "max_error",
                            "elevation_scaling"};

  this->add_port(
      gnode::Port("elevation", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("RGB",
                             gnode::direction::in,
                             dtype::dHeightMapRGB,
                             gnode::optional::yes));
}

void ExportAsset::compute()
{
  if (GET_ATTR_BOOL("auto_export"))
    this->write_file();
}

void ExportAsset::write_file()
{
  if (this->get_p_data("elevation"))
  {
    hmap::HeightMap *p_h = (hmap::HeightMap *)this->get_p_data("elevation");
    hmap::Array      array = p_h->to_array();

    std::string fname = GET_ATTR_FILENAME("fname");

    // if available export RGB to an image file
    std::string texture_fname = "";
    if (this->get_p_data("RGB"))
    {
      texture_fname = fname + ".png";
      ((hmap::HeightMapRGB *)this->get_p_data("RGB"))
          ->to_png_16bit(texture_fname);
    }

    hmap::export_asset(
        fname,
        array,
        (hmap::MeshType)GET_ATTR_MAPENUM("mesh_type"),
        (hmap::AssetExportFormat)GET_ATTR_MAPENUM("export_format"),
        GET_ATTR_FLOAT("elevation_scaling"),
        texture_fname,
        "", // TODO normal map
        GET_ATTR_FLOAT("max_error"));
  }
}

} // namespace hesiod::cnode
