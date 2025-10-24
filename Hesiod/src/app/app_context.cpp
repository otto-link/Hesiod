/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/app/app_context.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

void AppContext::json_from(nlohmann::json const &json)
{
  Logger::log()->trace("AppContext::json_from");

  if (json.contains("app_settings"))
    this->app_settings.json_from(json["app_settings"]);
  else
    Logger::log()->error("AppContext::json_from: could not parse app_settings");

  if (json.contains("style_settings"))
    this->style_settings.json_from(json["style_settings"]);
  else
    Logger::log()->error("AppContext::json_from: could not parse style_settings");
}

nlohmann::json AppContext::json_to() const
{
  Logger::log()->trace("AppContext::json_to");

  nlohmann::json json;
  json["app_settings"] = this->app_settings.json_to();
  json["style_settings"] = this->style_settings.json_to();
  return json;
}

} // namespace hesiod
