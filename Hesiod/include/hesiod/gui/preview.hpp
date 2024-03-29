/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QLabel>
#include <QWidget>

#include <QtNodes/NodeData>

#include "hesiod/model/model_config.hpp"

namespace hesiod
{

class Preview : public QWidget
{
  Q_OBJECT

public:
  Preview() = default;

  Preview(const ModelConfig *p_config, QtNodes::NodeData *p_data);

public Q_SLOTS:
  void update_image();

private:
  const ModelConfig *p_config;
  QtNodes::NodeData *p_data;
  QLabel            *label;
};

} // namespace hesiod