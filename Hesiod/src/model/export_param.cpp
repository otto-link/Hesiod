/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include "hesiod/logger.hpp"
#include "hesiod/model/export_param.hpp"

namespace hesiod
{

void ExportParam::dump()
{
  LOG->trace("ExportParam::dump");
  std::cout << this->json_to().dump(4) << "\n";
}

void ExportParam::json_from(nlohmann::json const &json)
{
  this->shape.x = json["shape.x"];
  this->shape.y = json["shape.y"];
  this->tiling.x = json["tiling.x"];
  this->tiling.y = json["tiling.y"];
  this->overlap = json["overlap"];
  this->export_path = json["export_path"].get<std::filesystem::path>();
  this->ids = json["ids"];
}

nlohmann::json ExportParam::json_to() const
{
  nlohmann::json json;
  json["shape.x"] = this->shape.x;
  json["shape.y"] = this->shape.y;
  json["tiling.x"] = this->tiling.x;
  json["tiling.y"] = this->tiling.y;
  json["overlap"] = this->overlap;
  json["export_path"] = this->export_path;
  json["ids"] = this->ids;
  return json;
}

} // namespace hesiod
