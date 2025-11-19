/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/export.hpp"
#include "highmap/gradient.hpp"
#include "highmap/tensor.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_export_asset_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "elevation");
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture");
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "normal map details");

  // attribute(s)
  node.add_attr<FilenameAttribute>("fname",
                                   "Export File",
                                   std::filesystem::path("export"),
                                   "*",
                                   true);

  node.add_attr<BoolAttribute>("auto_export", "Automatic Export", false);

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

    node.add_attr<EnumAttribute>("export_format",
                                 "Export Format:",
                                 export_format_map,
                                 default_export_format);
    node.add_attr<EnumAttribute>("mesh_type",
                                 "Mesh Type:",
                                 export_mesh_map,
                                 default_mesh_type);
  }

  node.add_attr<FloatAttribute>("max_error", "Max Error", 5e-4f, 0.f, 0.01f);
  node.add_attr<FloatAttribute>("elevation_scaling", "Elevation Scale", 0.2f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("detail_scaling", "Normal Map Scale", 1.f, 0.f, 4.f);
  node.add_attr<EnumAttribute>("blending_method",
                               "Blending Method:",
                               hmap::normal_map_blending_method_as_string);

  // attribute(s) order
  node.set_attr_ordered_key({"auto_export",
                             "fname",
                             "export_format",
                             "mesh_type",
                             "max_error",
                             "elevation_scaling",
                             "_SEPARATOR_",
                             "blending_method",
                             "detail_scaling"});

  // specialized GUI
  add_export_button(node);
}

void compute_export_asset_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_elev = node.get_value_ref<hmap::Heightmap>("elevation");

  if (p_elev && node.get_attr<BoolAttribute>("auto_export"))
  {
    hmap::HeightmapRGBA *p_color = node.get_value_ref<hmap::HeightmapRGBA>("texture");
    hmap::HeightmapRGBA *p_nmap = node.get_value_ref<hmap::HeightmapRGBA>(
        "normal map details");

    hmap::Array array = p_elev->to_array();
    std::string fname = node.get_attr<FilenameAttribute>("fname").string();

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
          node.get_attr<FloatAttribute>("detail_scaling"),
          (hmap::NormalMapBlendingMethod)node.get_attr<EnumAttribute>("blending_method"));
    }

    normal_map.to_png(nmap_fname, CV_16U);

    // --- export

    hmap::export_asset(
        fname,
        array,
        (hmap::MeshType)node.get_attr<EnumAttribute>("mesh_type"),
        (hmap::AssetExportFormat)node.get_attr<EnumAttribute>("export_format"),
        node.get_attr<FloatAttribute>("elevation_scaling"),
        texture_fname,
        nmap_fname,
        node.get_attr<FloatAttribute>("max_error"));
  }

  // not output, do not propagate
  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
