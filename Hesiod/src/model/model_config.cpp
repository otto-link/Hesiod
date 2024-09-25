/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/model_config.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

void ModelConfig::log_debug() const
{
  LOG->info("shape: {{{}, {}}}", shape.x, shape.y);
  LOG->info("tiling: {{{}, {}}}", tiling.x, tiling.y);
  LOG->info("overlap: {}", overlap);
}

void ModelConfig::json_from(nlohmann::json const &json)
{
  this->shape.x = json["shape.x"];
  this->shape.y = json["shape.y"];
  this->tiling.x = json["tiling.x"];
  this->tiling.y = json["tiling.y"];
  this->overlap = json["overlap"];
}

nlohmann::json ModelConfig::json_to() const
{
  nlohmann::json json;
  json["shape.x"] = this->shape.x;
  json["shape.y"] = this->shape.y;
  json["tiling.x"] = this->tiling.x;
  json["tiling.y"] = this->tiling.y;
  json["overlap"] = this->overlap;
  return json;
}

} // namespace hesiod
