/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QJsonObject>

#include "highmap/vector.hpp"
#include "macrologger.h"

#include "hesiod/model/model_utils.hpp"

#define HSD_PREVIEW_SHAPE 128

namespace hesiod
{

struct ModelConfig
{
  // --- model
  hmap::Vec2<int> shape = {4, 4};
  hmap::Vec2<int> tiling = {1, 1};
  float           overlap = 0.f;

  void log_debug() const
  {
    LOG_DEBUG("shape: {%d, %d}", shape.x, shape.y);
    LOG_DEBUG("tiling: {%d, %d}", tiling.x, tiling.y);
    LOG_DEBUG("overlap: %f", overlap);
    LOG_DEBUG("shape_preview: {%d, %d}", shape_preview.x, shape_preview.y);
  }

  void set_shape(hmap::Vec2<int> new_shape)
  {
    this->shape = new_shape;
    this->shape_preview = {std::min(HSD_PREVIEW_SHAPE, this->shape.x),
                           std::min(HSD_PREVIEW_SHAPE, this->shape.y)};
  }

  // --- GUI
  hmap::Vec2<int> shape_preview = {std::min(HSD_PREVIEW_SHAPE, shape.x),
                                   std::min(HSD_PREVIEW_SHAPE, shape.y)};

  // --- methods
  void load(QJsonObject const &p)
  {
    bool ret = true;

    hmap::Vec2<int> shape_load;
    ret &= convert_qjsonvalue_to_int(p["shape.x"], shape_load.x);
    ret &= convert_qjsonvalue_to_int(p["shape.y"], shape_load.y);
    this->set_shape(shape_load);

    ret &= convert_qjsonvalue_to_int(p["tiling.x"], this->tiling.x);
    ret &= convert_qjsonvalue_to_int(p["tiling.y"], this->tiling.y);
    ret &= convert_qjsonvalue_to_float(p["overlap"], this->overlap);
    if (!ret)
      LOG_ERROR("serialization in with WaveNbAttribute");
  }

  QJsonObject save() const
  {
    QJsonObject model_json;
    model_json["shape.x"] = QString::number(this->shape.x);
    model_json["shape.y"] = QString::number(this->shape.y);
    model_json["tiling.x"] = QString::number(this->tiling.x);
    model_json["tiling.y"] = QString::number(this->tiling.y);
    model_json["overlap"] = QString::number(this->overlap);
    return model_json;
  }
};

} // namespace hesiod