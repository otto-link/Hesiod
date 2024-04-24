/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QAction>
#include <QWidget>

#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>

#include "hesiod/gui/node_settings_widget.hpp"
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
  NodeEditorWidget() = default;

  NodeEditorWidget(std::string graph_id = "default_id", QWidget *parent = nullptr);

  hesiod::ModelConfig *get_model_config_ref() { return &this->model_config; }

  hesiod::HsdDataFlowGraphModel *get_model_ref() { return this->model.get(); }

  QtNodes::DataFlowGraphicsScene *get_scene_ref() { return this->scene.get(); }

Q_SIGNALS:
  void computingStarted(QtNodes::NodeId const node_id);

  void computingFinished(QtNodes::NodeId const node_id);

  void before_updating_model_configuration(std::string graph_id);

public Q_SLOTS:
  void clear();

  void load(std::string filename);

  void save(std::string filename);

  void save_screenshot(std::string filename);

  void update_model_configuration();

private:
  std::string graph_id;

  hesiod::ModelConfig model_config;

  std::unique_ptr<hesiod::HsdDataFlowGraphModel> model;

  std::unique_ptr<QtNodes::DataFlowGraphicsScene> scene;

  QtNodes::GraphicsView  *view;
  hesiod::Viewer2dWidget *viewer2d;
  hesiod::Viewer3dWidget *viewer3d;
  NodeSettingsWidget     *node_settings_widget;

  void toggle_widget_visibility(QWidget *widget, QPushButton *button);
};

} // namespace hesiod