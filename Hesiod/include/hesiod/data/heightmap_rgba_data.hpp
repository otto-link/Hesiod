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

class HeightMapRGBAData : public QtNodes::NodeData
{
public:
  HeightMapRGBAData() : name("HeightMapRGBAData") {}

  HeightMapRGBAData(const ModelConfig *p_config)
  {
    this->value.set_sto(p_config->shape, p_config->tiling, p_config->overlap);
  }

  hmap::HeightMapRGBA get_value() const { return this->value; }

  hmap::HeightMapRGBA *get_ref() { return &this->value; }

  void set_sto(hmap::Vec2<int> new_shape, hmap::Vec2<int> new_tiling, float new_overlap)
  {
    this->value.set_sto(new_shape, new_tiling, new_overlap);
  }

  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType{"HeightMapRGBAData", this->name};
  }

private:
  hmap::HeightMapRGBA value;
  QString             name;
};

} // namespace hesiod