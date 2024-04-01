/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QWidget>

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/NodeDelegateModel>

#include "hesiod/model/base_node.hpp"
#include "hesiod/model/graph_model_addon.hpp"
#include "hesiod/model/model_config.hpp"

namespace hesiod
{

class NodeSettingsWidget : public QWidget
{
  Q_OBJECT

public:
  NodeSettingsWidget() = default;

  NodeSettingsWidget(QtNodes::DataFlowGraphicsScene *p_scene, QWidget *parent = nullptr);

public Q_SLOTS:
  // void update_after_computing(QtNodes::NodeId const node_id);

  void update_layout(QtNodes::NodeId const nodeId);

private:
  QtNodes::DataFlowGraphicsScene *p_scene;
  HsdDataFlowGraphModel          *p_model;
  QWidget                        *parent;

  QtNodes::NodeId current_node_id = -1;

  QVBoxLayout *layout;
  QCheckBox   *checkbox_pin_node;
};

} // namespace hesiod