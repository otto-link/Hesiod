/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QAction>
#include <QWidget>

#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>

#include "hesiod/gui/viewer2d_widget.hpp"
#include "hesiod/gui/viewer3d_widget.hpp"
#include "hesiod/model/graph_model_addon.hpp"
#include "hesiod/model/model_config.hpp"

namespace hesiod
{

class NodeEditorWidget : public QWidget
{
  Q_OBJECT

public:
  hesiod::ModelConfig model_config;

  NodeEditorWidget() = default;

  NodeEditorWidget(hesiod::ModelConfig model_config, QWidget *parent = nullptr);

  hesiod::HsdDataFlowGraphModel *get_model_ref() { return this->model.get(); }

  QtNodes::DataFlowGraphicsScene *get_scene_ref() { return this->scene.get(); }

  Viewer2dWidget *get_viewer2d_ref() { return this->viewer2d.get(); }

  Viewer3dWidget *get_viewer3d_ref() { return this->viewer3d.get(); }

Q_SIGNALS:
  void computingStarted(QtNodes::NodeId const node_id);

  void computingFinished(QtNodes::NodeId const node_id);

public Q_SLOTS:
  void load() { this->get_scene_ref()->load(); }

  void save() { this->get_scene_ref()->save(); }

private:
  std::unique_ptr<hesiod::HsdDataFlowGraphModel>  model;
  std::unique_ptr<QtNodes::DataFlowGraphicsScene> scene;

  std::unique_ptr<hesiod::Viewer2dWidget> viewer2d;
  std::unique_ptr<hesiod::Viewer3dWidget> viewer3d;
};

} // namespace hesiod