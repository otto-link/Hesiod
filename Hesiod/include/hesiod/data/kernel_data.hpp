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

class KernelData : public QtNodes::NodeData
{
public:
  KernelData() : name("KernelData") {}

  hmap::Array get_value() const { return this->value; }

  hmap::Array *get_ref() { return &this->value; }

  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType{"KernelData", this->name};
  }

private:
  hmap::Array value;
  QString     name;
};

} // namespace hesiod