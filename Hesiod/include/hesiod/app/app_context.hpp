/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QApplication>
#include <QDir>
#include <QFileInfo>

#include "hesiod/app/app_settings.hpp"
#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/app/style_settings.hpp"
#include "hesiod/model/project_model.hpp"

namespace hesiod
{

class AppContext
{
public:
  AppContext() = default;

  // --- Settings management
  void           settings_json_from(nlohmann::json const &json);
  nlohmann::json settings_json_to() const;
  void           load_settings();
  void           save_settings() const;

  // --- Project management
  void new_project();
  void load_project_model(const std::string &fname);
  void save_project_model(const std::string &fname) const;

  // --- Data

  // global settings
  AppSettings   app_settings;
  StyleSettings style_settings;

  // project
  std::unique_ptr<ProjectModel> project_model;
};

// helpers

std::string get_config_file_path(const QString &app_name, bool portable_mode);
std::string get_config_file_path_auto(const QString &app_name);

} // namespace hesiod