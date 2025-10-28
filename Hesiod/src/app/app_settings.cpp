/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"

#include "hesiod/app/app_settings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

void AppSettings::json_from(nlohmann::json const &json)
{
  Logger::log()->trace("AppSettings::json_from");

  json_safe_get(json, "colors.bg_primary", colors.bg_primary);
  json_safe_get(json, "colors.bg_secondary", colors.bg_secondary);
  json_safe_get(json, "colors.text_primary", colors.text_primary);
  json_safe_get(json, "colors.text_disabled", colors.text_disabled);
  json_safe_get(json, "colors.accent", colors.accent);
  json_safe_get(json, "colors.border", colors.border);
  json_safe_get(json, "colors.hover", colors.hover);
  json_safe_get(json, "colors.pressed", colors.pressed);
  json_safe_get(json, "colors.separator", colors.separator);

  json_safe_get(json, "global.icon_path", global.icon_path);
  json_safe_get(json,
                "global.default_startup_project_file",
                global.default_startup_project_file);
  json_safe_get(json,
                "global.enable_texture_downloader",
                global.enable_texture_downloader);

  // OpenCL device
  {
    json_safe_get(json, "node_editor.gpu_device_name", node_editor.gpu_device_name);

    // if it's empty, let the one choosed by default by the OpenCL
    // wrapper
    if (!node_editor.gpu_device_name.empty())
    {
      auto cl_device_map = clwrapper::DeviceManager::get_instance()
                               .get_available_devices();

      for (auto &[device_id, device_name] : cl_device_map)
      {
        if (device_name == node_editor.gpu_device_name)
        {
          if (clwrapper::DeviceManager::get_instance().set_device(device_id))
            clwrapper::KernelManager::get_instance().build_program();
          else
            Logger::log()->error("OpenCL device selection failed");
        }
      }
    }
  }

  json_safe_get(json, "node_editor.default_resolution", node_editor.default_resolution);
  json_safe_get(json, "node_editor.default_tiling", node_editor.default_tiling);
  json_safe_get(json, "node_editor.default_overlap", node_editor.default_overlap);
  json_safe_get(json, "node_editor.preview_w", node_editor.preview_w);
  json_safe_get(json, "node_editor.preview_h", node_editor.preview_h);
  json_safe_get<std::string>(json, "node_editor.doc_path", node_editor.doc_path);
  json_safe_get(json,
                "node_editor.position_delta_when_duplicating_node",
                node_editor.position_delta_when_duplicating_node);
  json_safe_get(json, "node_editor.auto_layout_dx", node_editor.auto_layout_dx);
  json_safe_get(json, "node_editor.auto_layout_dy", node_editor.auto_layout_dy);
  json_safe_get(json,
                "node_editor.show_node_settings_pan",
                node_editor.show_node_settings_pan);

  json_safe_get(json, "viewer.width", viewer.width);
  json_safe_get(json, "viewer.height", viewer.height);

  // window
  {
    json_safe_get(json, "window.x", window.x);
    json_safe_get(json, "window.y", window.y);
    json_safe_get(json, "window.w", window.w);
    json_safe_get(json, "window.h", window.h);
    json_safe_get(json, "window.progress_bar_width", window.progress_bar_width);
    json_safe_get(json, "window.gm_x", window.gm_x);
    json_safe_get(json, "window.gm_y", window.gm_y);
    json_safe_get(json, "window.gm_w", window.gm_w);
    json_safe_get(json, "window.gm_h", window.gm_h);

    json_safe_get(json,
                  "window.show_graph_manager_widget",
                  window.show_graph_manager_widget);
    json_safe_get(json,
                  "window.show_texture_downloader_widget",
                  window.show_texture_downloader_widget);

    json_safe_get(json,
                  "window.open_graph_manager_at_startup",
                  window.open_graph_manager_at_startup);

    int64_t autosave_timer_value = 0;
    json_safe_get(json, "window.autosave_timer", autosave_timer_value);

    window.autosave_timer = std::chrono::milliseconds(autosave_timer_value);

    json_safe_get(json,
                  "window.open_viewport_at_startup",
                  window.open_viewport_at_startup);
    json_safe_get(json, "window.save_backup_file", window.save_backup_file);
  }
}

nlohmann::json AppSettings::json_to() const
{
  Logger::log()->trace("AppSettings::json_to");

  nlohmann::json json;

  json["colors.bg_primary"] = colors.bg_primary.name().toStdString();
  json["colors.bg_secondary"] = colors.bg_secondary.name().toStdString();
  json["colors.text_primary"] = colors.text_primary.name().toStdString();
  json["colors.text_disabled"] = colors.text_disabled.name().toStdString();
  json["colors.accent"] = colors.accent.name().toStdString();
  json["colors.border"] = colors.border.name().toStdString();
  json["colors.hover"] = colors.hover.name().toStdString();
  json["colors.pressed"] = colors.pressed.name().toStdString();
  json["colors.separator"] = colors.separator.name().toStdString();

  json["global.icon_path"] = global.icon_path;
  json["global.default_startup_project_file"] = global.default_startup_project_file;
  json["global.enable_texture_downloader"] = global.enable_texture_downloader;

  json["node_editor.gpu_device_name"] = node_editor.gpu_device_name;
  json["node_editor.default_resolution"] = node_editor.default_resolution;
  json["node_editor.default_tiling"] = node_editor.default_tiling;
  json["node_editor.default_overlap"] = node_editor.default_overlap;
  json["node_editor.preview_w"] = node_editor.preview_w;
  json["node_editor.preview_h"] = node_editor.preview_h;
  json["node_editor.doc_path"] = node_editor.doc_path;
  json["node_editor.position_delta_when_duplicating_node"] =
      node_editor.position_delta_when_duplicating_node;
  json["node_editor.auto_layout_dx"] = node_editor.auto_layout_dx;
  json["node_editor.auto_layout_dy"] = node_editor.auto_layout_dy;
  json["node_editor.show_node_settings_pan"] = node_editor.show_node_settings_pan;

  json["viewer.width"] = viewer.width;
  json["viewer.height"] = viewer.height;

  json["window.x"] = window.x;
  json["window.y"] = window.y;
  json["window.w"] = window.w;
  json["window.h"] = window.h;
  json["window.progress_bar_width"] = window.progress_bar_width;
  json["window.gm_x"] = window.gm_x;
  json["window.gm_y"] = window.gm_y;
  json["window.gm_w"] = window.gm_w;
  json["window.gm_h"] = window.gm_h;
  json["window.show_graph_manager_widget"] = window.show_graph_manager_widget;
  json["window.show_texture_downloader_widget"] = window.show_texture_downloader_widget;
  json["window.open_graph_manager_at_startup"] = window.open_graph_manager_at_startup;
  json["window.open_viewport_at_startup"] = window.open_viewport_at_startup;
  json["window.autosave_timer"] = static_cast<int>(window.autosave_timer.count());
  json["window.save_backup_file"] = window.save_backup_file;

  return json;
}

} // namespace hesiod
