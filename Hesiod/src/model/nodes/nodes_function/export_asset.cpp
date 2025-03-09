/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/export.hpp"
#include "highmap/gradient.hpp"
#include "highmap/tensor.hpp"

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
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "elevation");
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture");
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "normal map details");

  // attribute(s)
  ADD_ATTR(FilenameAttribute, "fname", std::filesystem::path("export"), "*", true);
  ADD_ATTR(BoolAttribute, "auto_export", true);

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

    ADD_ATTR(EnumAttribute, "export_format", export_format_map, default_export_format);
    ADD_ATTR(EnumAttribute, "mesh_type", export_mesh_map, default_mesh_type);
  }

  ADD_ATTR(FloatAttribute, "max_error", 5e-4f, 0.f, 0.01f);
  ADD_ATTR(FloatAttribute, "elevation_scaling", 0.2f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "detail_scaling", 1.f, 0.f, 4.f);
  ADD_ATTR(EnumAttribute, "blending_method", hmap::normal_map_blending_method_as_string);

  // attribute(s) order
  p_node->set_attr_ordered_key({"auto_export",
                                "fname",
                                "export_format",
                                "mesh_type",
                                "max_error",
                                "elevation_scaling",
                                "_SEPARATOR_",
                                "blending_method",
                                "detail_scaling"});
}

void compute_export_asset_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_elev = p_node->get_value_ref<hmap::Heightmap>("elevation");

  if (p_elev)
  {
    hmap::HeightmapRGBA *p_color = p_node->get_value_ref<hmap::HeightmapRGBA>("texture");
    hmap::HeightmapRGBA *p_nmap = p_node->get_value_ref<hmap::HeightmapRGBA>(
        "normal map details");

    hmap::Array array = p_elev->to_array();
    std::string fname = GET("fname", FilenameAttribute).string();

    // --- if available export RGBA to an image file

    std::string texture_fname = "";

    if (p_color)
    {
      texture_fname = fname + ".png";
      p_color->to_png(texture_fname, CV_16U);
    }

    // --- blend details normal map with base normal map

    std::string nmap_fname = fname + "_nmap.png";

    // TODO optimize / distribute this
    hmap::Tensor nvec = hmap::normal_map(p_elev->to_array());

    hmap::HeightmapRGBA normal_map = hmap::HeightmapRGBA(p_elev->shape,
                                                         p_elev->tiling,
                                                         p_elev->overlap,
                                                         nvec.get_slice(0),
                                                         nvec.get_slice(1),
                                                         nvec.get_slice(2),
                                                         hmap::Array(p_elev->shape, 1.f));

    if (p_nmap)
    {
      normal_map = hmap::mix_normal_map_rgba(
          normal_map,
          *p_nmap,
          GET("detail_scaling", FloatAttribute),
          (hmap::NormalMapBlendingMethod)GET("blending_method", EnumAttribute));
    }

    normal_map.to_png(nmap_fname, CV_16U);

    // --- export

    hmap::export_asset(fname,
                       array,
                       (hmap::MeshType)GET("mesh_type", EnumAttribute),
                       (hmap::AssetExportFormat)GET("export_format", EnumAttribute),
                       GET("elevation_scaling", FloatAttribute),
                       texture_fname,
                       nmap_fname,
                       GET("max_error", FloatAttribute));
  }

  // not output, do not propagate
  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
