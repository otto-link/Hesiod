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
  LOG->trace("Config::json_from");

  this->window
      .open_graph_manager_at_startup = json["window.open_graph_manager_at_startup"];
  this->window.open_viewport_at_startup = json["window.open_viewport_at_startup"];
  this->window.autosave_timer = std::chrono::milliseconds(json["window.autosave_timer"]);
  this->window.save_backup_file = json["window.save_backup_file"];
}

nlohmann::json Config::json_to() const
{
  LOG->trace("Config::json_to");

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
  LOG->trace("Config::load_from_file: {}", fname);
  nlohmann::json json = json_from_file(fname);
  this->json_from(json);
}

void Config::save_to_file(const std::string &fname) const
{
  LOG->trace("Config::save_to_file: {}", fname);
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
