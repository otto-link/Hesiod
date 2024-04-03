/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QtCore/QString>
#include <QtNodes/NodeData>

#include "highmap/geometry.hpp"

namespace hesiod
{

class CloudData : public QtNodes::NodeData
{
public:
  CloudData() : name("CloudData") {}

  hmap::Cloud get_value() const { return this->value; }

  hmap::Cloud *get_ref() { return &this->value; }

  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType{"CloudData", this->name};
  }

private:
  hmap::Cloud value;
  QString     name;
};

} // namespace hesiod