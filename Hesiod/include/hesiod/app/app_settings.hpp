/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QColor>

#include "nlohmann/json.hpp"

#include "highmap/heightmap.hpp"

namespace hesiod
{

struct AppSettings
{
  AppSettings() = default;

  // --- Serialization
  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;

  // --- Data
  struct Colors
  {
    QColor bg_primary{"#2B2B2B"};
    QColor bg_secondary{"#4B4B4B"};
    QColor text_primary{"#F4F4F5"};
    QColor text_disabled{"#3C3C3C"};
    QColor accent{"#4772B3"};
    QColor border{"#5B5B5B"};
    QColor hover{"#8B8B8B"};
    QColor pressed{"#ABABAB"};
    QColor separator{"#ABABAB"};
  } colors;

  struct Global
  {
    std::string icon_path = "data/hesiod_icon.png";
    std::string default_startup_project_file = "data/default.hsd";
    std::string quick_start_html_file = "data/quick_start.html";
    bool        enable_texture_downloader = true;
  } global;

  struct NodeEditor
  {
    std::string         gpu_device_name = ""; // let CLWrapper decides
    hmap::TransformMode hmap_transform_mode_cpu = hmap::TransformMode::DISTRIBUTED;
    hmap::TransformMode hmap_transform_mode_gpu = hmap::TransformMode::SINGLE_ARRAY;
    int                 default_resolution = 1024;
    int                 default_tiling = 4;
    float               default_overlap = 0.5f;
    int                 preview_w = 128;
    int                 preview_h = 128;
    std::string         doc_path = "data/node_documentation.json";
    float               position_delta_when_duplicating_node = 200.f;
    float               auto_layout_dx = 256.f;
    float               auto_layout_dy = 384.f;
    bool                show_node_settings_pan = false;
  } node_editor;

  struct Viewer
  {
    int width = 512;
    int height = 512;
  } viewer;

  struct Window // main window
  {
    int  x = 0;
    int  y = 0;
    int  w = 1024;
    int  h = 1024;
    bool show_graph_manager_widget = false;
    bool show_texture_downloader_widget = false;

    int gm_x = 0; // graph manager geometry
    int gm_y = 0;
    int gm_w = 1024;
    int gm_h = 1024;

    // TODO remove
    bool                      open_graph_manager_at_startup = false;
    bool                      open_viewport_at_startup = false;
    std::chrono::milliseconds autosave_timer{60000}; // 60 seconds
    bool                      save_backup_file = true;
  } window;
};

} // namespace hesiod