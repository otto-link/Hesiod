/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/config.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

// Initialize the static member
std::shared_ptr<Config> Config::instance = nullptr;

void Config::json_from(nlohmann::json const &json)
{
  Logger::log()->trace("Config::json_from");

  json_safe_get(json,
                "window.open_graph_manager_at_startup",
                this->window.open_graph_manager_at_startup);

  int64_t autosave_timer_value = 0;
  json_safe_get(json, "window.autosave_timer", autosave_timer_value);

  this->window.autosave_timer = std::chrono::milliseconds(autosave_timer_value);

  json_safe_get(json,
                "window.open_viewport_at_startup",
                this->window.open_viewport_at_startup);
  json_safe_get(json, "window.save_backup_file", this->window.save_backup_file);
}

nlohmann::json Config::json_to() const
{
  Logger::log()->trace("Config::json_to");

  nlohmann::json json;

  json["window.open_graph_manager_at_startup"] = this->window
                                                     .open_graph_manager_at_startup;
  json["window.open_viewport_at_startup"] = this->window.open_viewport_at_startup;
  json["window.autosave_timer"] = static_cast<int>(this->window.autosave_timer.count());
  json["window.save_backup_file"] = this->window.save_backup_file;
  return json;
}

void Config::load_from_file(const std::string &fname)
{
  Logger::log()->trace("Config::load_from_file: {}", fname);
  nlohmann::json json = json_from_file(fname);
  this->json_from(json);
}

void Config::save_to_file(const std::string &fname) const
{
  Logger::log()->trace("Config::save_to_file: {}", fname);
  nlohmann::json json = this->json_to();
  json_to_file(json, fname);
}

std::shared_ptr<Config> &Config::get_config()
{
  if (!instance)
    instance = std::make_shared<Config>();
  return instance;
}

} // namespace hesiod
