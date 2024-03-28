/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModel>

#include "hesiod/model/model_config.hpp"

namespace hesiod
{

class HViewer2d : public QWidget
{
  Q_OBJECT

public:
  HViewer2d() = default;

  HViewer2d(ModelConfig *p_config, QtNodes::DataFlowGraphModel *p_model);

public Q_SLOTS:
  void update_viewport(QtNodes::NodeId const nodeId);

private:
  ModelConfig                 *p_config;
  QtNodes::DataFlowGraphModel *p_model;

  QLabel      *label;
  QGridLayout *layout;
};

} // namespace hesiod