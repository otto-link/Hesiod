/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/model_config.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

void ModelConfig::log_debug() const
{
  Logger::log()->trace("shape: {{{}, {}}}", shape.x, shape.y);
  Logger::log()->trace("tiling: {{{}, {}}}", tiling.x, tiling.y);
  Logger::log()->trace("overlap: {}", overlap);
}

void ModelConfig::json_from(nlohmann::json const &json)
{
  this->shape.x = json["shape.x"];
  this->shape.y = json["shape.y"];
  this->tiling.x = json["tiling.x"];
  this->tiling.y = json["tiling.y"];
  this->overlap = json["overlap"];
  this->hmap_transform_mode_cpu = json["hmap_transform_mode_cpu"];
  this->hmap_transform_mode_gpu = json["hmap_transform_mode_gpu"];
}

nlohmann::json ModelConfig::json_to() const
{
  nlohmann::json json;
  json["shape.x"] = this->shape.x;
  json["shape.y"] = this->shape.y;
  json["tiling.x"] = this->tiling.x;
  json["tiling.y"] = this->tiling.y;
  json["overlap"] = this->overlap;
  json["hmap_transform_mode_cpu"] = this->hmap_transform_mode_cpu;
  json["hmap_transform_mode_gpu"] = this->hmap_transform_mode_gpu;
  return json;
}

} // namespace hesiod
