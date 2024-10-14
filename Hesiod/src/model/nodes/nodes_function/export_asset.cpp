/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/export.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_export_asset_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "elevation");
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::IN, "texture");

  // attribute(s)
  p_node->add_attr<BoolAttribute>("auto_export", true, "auto_export");
  p_node->add_attr<FilenameAttribute>("fname", "export", "fname");

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

    p_node->add_attr<MapEnumAttribute>("export_format",
                                       default_export_format,
                                       export_format_map,
                                       "export_format");
    p_node->add_attr<MapEnumAttribute>("mesh_type",
                                       default_mesh_type,
                                       export_mesh_map,
                                       "mesh_type");
  }

  p_node->add_attr<FloatAttribute>("max_error", 5e-4f, 0.f, 0.01f, "max_error");
  p_node->add_attr<FloatAttribute>("elevation_scaling",
                                   0.2f,
                                   0.f,
                                   1.f,
                                   "elevation_scaling");

  // attribute(s) order
  p_node->set_attr_ordered_key({"auto_export",
                                "fname",
                                "export_format",
                                "mesh_type",
                                "max_error",
                                "elevation_scaling"});
}

void compute_export_asset_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_elev = p_node->get_value_ref<hmap::HeightMap>("elevation");

  if (p_elev)
  {
    hmap::HeightMapRGBA *p_color = p_node->get_value_ref<hmap::HeightMapRGBA>("texture");

    hmap::Array array = p_elev->to_array();
    std::string fname = GET("fname", FilenameAttribute).string();

    // if available export RGBA to an image file
    std::string texture_fname = "";

    if (p_color)
    {
      texture_fname = fname + ".png";
      p_color->to_png(texture_fname, CV_16U);
    }

    hmap::export_asset(fname,
                       array,
                       (hmap::MeshType)GET("mesh_type", MapEnumAttribute),
                       (hmap::AssetExportFormat)GET("export_format", MapEnumAttribute),
                       GET("elevation_scaling", FloatAttribute),
                       texture_fname,
                       "", // TODO normal map
                       GET("max_error", FloatAttribute));
  }

  // not output, do not propagate
  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
