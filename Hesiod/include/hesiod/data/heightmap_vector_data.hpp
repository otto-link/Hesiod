/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QtCore/QString>
#include <QtNodes/NodeData>

#include "highmap/heightmap.hpp"

namespace hesiod
{

class HeightMapVectorData : public QtNodes::NodeData
{
public:
  HeightMapVectorData() : name("HeightMapVectorData") {}

  std::vector<hmap::HeightMap> get_value() const { return this->value; }

  std::vector<hmap::HeightMap> *get_ref() { return &this->value; }

  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType{"HeightMapVectorData", this->name};
  }

private:
  std::vector<hmap::HeightMap> value;
  QString                      name;
};

} // namespace hesiod