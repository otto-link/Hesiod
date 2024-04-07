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

class Viewer3dWidget : public QWidget
{
  Q_OBJECT

public:
  Viewer3dWidget() = default;

  Viewer3dWidget(ModelConfig                    *p_config,
                 QtNodes::DataFlowGraphicsScene *p_scene,
                 QWidget                        *parent = nullptr);

Q_SIGNALS:
  void resized(int width, int height);

public Q_SLOTS:
  void on_node_selected(QtNodes::NodeId const node_id);

  void reset();

  void update_after_computing(QtNodes::NodeId const node_id);

  void update_viewport(QtNodes::NodeId const nodeId);

  void update_viewport();

protected:
  void resizeEvent(QResizeEvent *event) override;

private:
  ModelConfig                    *p_config;
  QtNodes::DataFlowGraphicsScene *p_scene;
  HsdDataFlowGraphModel          *p_model;
  QWidget                        *parent;

  QtNodes::NodeId    current_node_id = -1;
  QtNodes::NodeData *p_data = nullptr;

  QCheckBox    *checkbox_pin_node;
  HmapGLViewer *gl_viewer = nullptr;
};

} // namespace hesiod