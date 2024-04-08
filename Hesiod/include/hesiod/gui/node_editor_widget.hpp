/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QAction>
#include <QWidget>

#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>

#include "hesiod/model/graph_model_addon.hpp"
#include "hesiod/model/model_config.hpp"

namespace hesiod
{

class NodeEditorWidget : public QWidget
{
  Q_OBJECT

public:
  NodeEditorWidget() = default;

  NodeEditorWidget(hesiod::ModelConfig *p_model_config, QWidget *parent = nullptr);

  hesiod::ModelConfig *get_model_config_ref() { return this->p_model_config; }

  hesiod::HsdDataFlowGraphModel *get_model_ref() { return this->model.get(); }

  QtNodes::DataFlowGraphicsScene *get_scene_ref() { return this->scene.get(); }

Q_SIGNALS:
  void computingStarted(QtNodes::NodeId const node_id);

  void computingFinished(QtNodes::NodeId const node_id);

public Q_SLOTS:
  void load();

  void save();

private:
  hesiod::ModelConfig *p_model_config;

  std::unique_ptr<hesiod::HsdDataFlowGraphModel> model;

  std::unique_ptr<QtNodes::DataFlowGraphicsScene> scene;
};

} // namespace hesiod