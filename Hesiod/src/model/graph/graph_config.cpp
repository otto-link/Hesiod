/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/graph/graph_config.hpp"
#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

GraphConfig::GraphConfig()
{
  Logger::log()->trace("GraphConfig::GraphConfig");

  AppContext &ctx = HSD_CTX;

  this->shape = {ctx.app_settings.node_editor.default_resolution,
                 ctx.app_settings.node_editor.default_resolution};
  this->tiling = {ctx.app_settings.node_editor.default_tiling,
                  ctx.app_settings.node_editor.default_tiling};
  this->overlap = ctx.app_settings.node_editor.default_overlap;

  this->update_parameters();
}

void GraphConfig::log_debug() const
{
  Logger::log()->debug("shape: {{{}, {}}}", shape.x, shape.y);
  Logger::log()->debug("tiling: {{{}, {}}}", tiling.x, tiling.y);
  Logger::log()->debug("overlap: {}", overlap);
  Logger::log()->debug("tile_shape: {} {}", tile_shape.x, tile_shape.y);
  Logger::log()->debug("halo: {}", halo);
}

void GraphConfig::json_from(nlohmann::json const &json)
{
  this->shape.x = json["shape.x"];
  this->shape.y = json["shape.y"];
  this->tiling.x = json["tiling.x"];
  this->tiling.y = json["tiling.y"];
  this->overlap = json["overlap"];

  this->update_parameters();
}

nlohmann::json GraphConfig::json_to() const
{
  nlohmann::json json;
  json["shape.x"] = this->shape.x;
  json["shape.y"] = this->shape.y;
  json["tiling.x"] = this->tiling.x;
  json["tiling.y"] = this->tiling.y;
  json["overlap"] = this->overlap;
  return json;
}

void GraphConfig::set_shape(const hmap::Vec2<int> &new_shape)
{
  this->shape = new_shape;
  this->update_parameters();
}

void GraphConfig::update_parameters()
{
  int w = int(this->shape.x / this->tiling.x);
  int h = int(this->shape.y / this->tiling.y);

  this->tile_shape = {w, h};
  this->halo = std::max(int(this->overlap * w), int(this->overlap * h));

  this->log_debug();
}

} // namespace hesiod
