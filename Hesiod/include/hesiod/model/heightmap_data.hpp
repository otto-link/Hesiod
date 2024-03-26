/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QtCore/QString>
#include <QtNodes/NodeData>

#include "highmap/heightmap.hpp"

#include "hesiod/model/model_config.hpp"

namespace hesiod
{

class HeightMapData : public QtNodes::NodeData
{
public:
  HeightMapData() : name("HeightMapData") {}

  HeightMapData(const ModelConfig &config)
  {
    this->value.set_sto(config.shape, config.tiling, config.overlap);
  }

  // HeightMapData(const hmap::HeightMap value) : value(value), name("HeightMapData") {}

  //  HeightMapData(const hmap::HeightMap value, const QString name)
  //      : value(value), name(name)
  //  {
  //  }

  hmap::HeightMap get_value() const { return this->value; }

  hmap::HeightMap *get_ref() { return &this->value; }

  void set_sto(hmap::Vec2<int> new_shape, hmap::Vec2<int> new_tiling, float new_overlap)
  {
    this->value.set_sto(new_shape, new_tiling, new_overlap);
  }

  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType{"HeightMapData", this->name};
  }

private:
  hmap::HeightMap value;
  QString         name;
};

} // namespace hesiod