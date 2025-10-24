/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

HesiodApplication::HesiodApplication(int &argc, char **argv) : QApplication(argc, argv)
{
  Logger::log()->trace("HesiodApplication::HesiodApplication");

  this->load_settings();
}

HesiodApplication::~HesiodApplication()
{
  Logger::log()->trace("HesiodApplication::~HesiodApplication");

  this->save_settings();
}

AppContext &HesiodApplication::get_context() { return context; }

const AppContext &HesiodApplication::get_context() const { return context; }

QApplication &HesiodApplication::get_qapp() { return *static_cast<QApplication *>(this); }

void HesiodApplication::load_settings()
{
  Logger::log()->trace("HesiodApplication::load_settings");

  std::string    fname = get_config_file_path_auto("hesiod");
  nlohmann::json json = json_from_file(fname);

  if (json.contains("app_settings"))
    this->get_context().app_settings.json_from(json["app_settings"]);
  else
    Logger::log()->error("HesiodApplication::load_settings: no app_settings entry");
}

void HesiodApplication::save_settings() const
{
  Logger::log()->trace("HesiodApplication::save_settings");

  // put everything into a json and dump it
  std::string    fname = get_config_file_path_auto("hesiod");
  bool           merge_with_existing_content = true;
  nlohmann::json json;

  json["app_settings"] = this->get_context().app_settings.json_to();

  json_to_file(json, fname, merge_with_existing_content);
}

// --- HELPERS ---

std::string get_config_file_path(const QString &app_name, bool portable_mode)
{
  QString path;

  if (portable_mode)
  {
    // Portable mode → store config next to the executable
    QDir app_dir(QCoreApplication::applicationDirPath());
    path = app_dir.filePath(app_name + ".json");
  }
  else
  {
    // Standard per-user config
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (dir.isEmpty())
    {
      // Fallback: use AppData if ConfigLocation unavailable
      dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }

    QDir().mkpath(dir); // ensure the directory exists
    path = dir + QDir::separator() + app_name + ".json";
  }

  return path.toStdString();
}

std::string get_config_file_path_auto(const QString &app_name)
{
  QDir    app_dir(QCoreApplication::applicationDirPath());
  QString portable_path = app_dir.filePath(app_name + ".json");
  QString portable_flag = app_dir.filePath("portable.flag");

  bool use_portable = QFileInfo::exists(portable_path) ||
                      QFileInfo::exists(portable_flag);

  return get_config_file_path(app_name, use_portable);
}

} // namespace hesiod
