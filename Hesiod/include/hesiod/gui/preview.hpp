/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QLabel>
#include <QWidget>

#include "hesiod/model/base_node.hpp"

namespace hesiod
{

class BaseNode; // forward decl.

class Preview : public QWidget
{
  Q_OBJECT

public:
  Preview() = default;

  Preview(BaseNode *p_node);

public Q_SLOTS:
  void update_image();

private:
  BaseNode *p_node;
  QLabel   *label;
};

} // namespace hesiod