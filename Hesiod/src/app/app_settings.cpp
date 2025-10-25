/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
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

  json_safe_get(json,
                "node_editor.hmap_transform_mode_cpu",
                node_editor.hmap_transform_mode_cpu);
  json_safe_get(json,
                "node_editor.hmap_transform_mode_gpu",
                node_editor.hmap_transform_mode_gpu);
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

  json_safe_get(json, "viewer.width", viewer.width);
  json_safe_get(json, "viewer.height", viewer.height);

  // window
  {
    json_safe_get(json,
                  "window.open_graph_manager_at_startup",
                  window.open_graph_manager_at_startup);

    int64_t autosave_timer_value = 0;
    json_safe_get(json, "window.autosave_timer", autosave_timer_value);

    this->window.autosave_timer = std::chrono::milliseconds(autosave_timer_value);

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

  json["node_editor.hmap_transform_mode_cpu"] = node_editor.hmap_transform_mode_cpu;
  json["node_editor.hmap_transform_mode_gpu"] = node_editor.hmap_transform_mode_gpu;
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

  json["viewer.width"] = viewer.width;
  json["viewer.height"] = viewer.height;

  json["window.open_graph_manager_at_startup"] = window.open_graph_manager_at_startup;
  json["window.open_viewport_at_startup"] = window.open_viewport_at_startup;
  json["window.autosave_timer"] = static_cast<int>(window.autosave_timer.count());
  json["window.save_backup_file"] = window.save_backup_file;

  return json;
}

} // namespace hesiod
