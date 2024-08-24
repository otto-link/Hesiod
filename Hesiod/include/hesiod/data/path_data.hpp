/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QtCore/QString>
#include <QtNodes/NodeData>

#include "highmap/geometry/path.hpp"

namespace hesiod
{

class PathData : public QtNodes::NodeData
{
public:
  PathData() : name("PathData") {}

  hmap::Path get_value() const { return this->value; }

  hmap::Path *get_ref() { return &this->value; }

  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType{"PathData", this->name};
  }

private:
  hmap::Path value;
  QString    name;
};

} // namespace hesiod
