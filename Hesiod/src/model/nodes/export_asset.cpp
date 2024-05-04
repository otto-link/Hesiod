/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/io.hpp"

#include "hesiod/model/nodes.hpp"

namespace hesiod
{

ExportAsset::ExportAsset(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("ExportAsset::ExportAsset");

  // model
  this->node_caption = "ExportAsset";

  // inputs
  this->input_captions = {"elevation", "texture"};
  this->input_types = {HeightMapData().type(), HeightMapRGBAData().type()};

  // outputs
  this->output_captions = {};
  this->output_types = {};

  // attributes
  this->attr["auto_export"] = NEW_ATTR_BOOL(false);
  this->attr["fname"] = NEW_ATTR_FILENAME("export");

  {
    // generate enumerate mappings
    std::map<std::string, int> export_format_map = {};
    std::map<std::string, int> export_mesh_map = {};

    for (auto &[export_id, export_infos] : hmap::asset_export_format_as_string)
      export_format_map[export_infos[0]] = (int)export_id;

    for (auto &[mesh_id, mesh_infos] : hmap::mesh_type_as_string)
      export_mesh_map[mesh_infos] = (int)mesh_id;

    std::string default_export_format = "GL Transmission Format v. 2 (binary) - *.glb";
    std::string default_mesh_type = "triangles";

    this->attr["export_format"] = NEW_ATTR_MAPENUM(export_format_map,
                                                   default_export_format);
    this->attr["mesh_type"] = NEW_ATTR_MAPENUM(export_mesh_map, default_mesh_type);
  }

  this->attr["max_error"] = NEW_ATTR_FLOAT(5e-4f, 0.f, 0.01f, "%.4f");
  this->attr["elevation_scaling"] = NEW_ATTR_FLOAT(0.2f, 0.f, 1.f);

  this->attr_ordered_key = {"auto_export",
                            "fname",
                            "export_format",
                            "mesh_type",
                            "max_error",
                            "elevation_scaling"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->compute();
  }

  // documentation
  this->description = "ExportAsset exporting both the heightmap and texture to a single "
                      "asset.";

  this->input_descriptions = {"Data values for elevation.", "Data values for color."};
  this->output_descriptions = {};

  this->attribute_descriptions["auto_export"] =
      "Decides whether the export is automatically performed when the node is updated.";
  this->attribute_descriptions["fname"] = "Export file name.";
  this->attribute_descriptions["export_format"] = "Export format.";
  this->attribute_descriptions["mesh_type"] = "Mesh type for the geometry.";
  this->attribute_descriptions
      ["max_error"] = "Maximum error (for optimized triangulated mesh).";
  this->attribute_descriptions["elevation_scaling"] = "Elevation scaling.";
}

std::shared_ptr<QtNodes::NodeData> ExportAsset::outData(
    QtNodes::PortIndex /* port_index */)
{
  // no output, return a dummy pointer
  return std::shared_ptr<QtNodes::NodeData>();
}

void ExportAsset::setInData(std::shared_ptr<QtNodes::NodeData> data,
                            QtNodes::PortIndex                 port_index)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  switch (port_index)
  {
  case 0:
    this->elevation = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->color = std::dynamic_pointer_cast<HeightMapRGBAData>(data);
  }

  this->compute();
}

// --- computing

void ExportAsset::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap     *p_elev = HSD_GET_POINTER(this->elevation);
  hmap::HeightMapRGBA *p_color = HSD_GET_POINTER(this->color);

  if (p_elev)
  {
    hmap::Array array = p_elev->to_array();

    std::string fname = GET_ATTR_FILENAME("fname");

    // if available export RGBA to an image file
    std::string texture_fname = "";

    if (p_color)
    {
      texture_fname = fname + ".png";
      p_color->to_png_16bit(texture_fname);
    }

    hmap::export_asset(fname,
                       array,
                       (hmap::MeshType)GET_ATTR_MAPENUM("mesh_type"),
                       (hmap::AssetExportFormat)GET_ATTR_MAPENUM("export_format"),
                       GET_ATTR_FLOAT("elevation_scaling"),
                       texture_fname,
                       "", // TODO normal map
                       GET_ATTR_FLOAT("max_error"));
  }

  // not output, do not propagate
  Q_EMIT this->computingFinished();
}

} // namespace hesiod
