/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
typedef unsigned int uint;
#include <iostream>

#include <QApplication>
#include <QMenuBar>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWidget>

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModelRegistry>

#include "highmap/vector.hpp"
#include "macrologger.h"

#include "hesiod/gui/h_viewer2d.hpp"
#include "hesiod/model/model_config.hpp"
#include "hesiod/model/nodes.hpp"

static std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registerDataModels(
    hesiod::ModelConfig &config)
{
  auto ret = std::make_shared<QtNodes::NodeDelegateModelRegistry>();

  ret->registerModel<hesiod::GammaCorrection>(
      [&config]() { return std::make_unique<hesiod::GammaCorrection>(config); },
      "Filters");

  ret->registerModel<hesiod::HeightMapToMask>(
      [&config]() { return std::make_unique<hesiod::HeightMapToMask>(config); },
      "Converters");

  ret->registerModel<hesiod::Noise>([&config]()
                                    { return std::make_unique<hesiod::Noise>(config); },
                                    "Primitives");

  return ret;
}

static void set_style()
{
  QtNodes::ConnectionStyle::setConnectionStyle(
      R"(
  {
    "ConnectionStyle": {
      "UseDataDefinedColors": true
    }
  }
  )");
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  app.setStyleSheet(R"(
QSlider::groove:horizontal { 
	background: #C9CDD0;
	height: 16px; 
	border-radius: 4px;
}

QSlider::handle:horizontal { 
	background-color: #51A0D5; 
	width: 16px; 
	height: 16px; 
	border-radius: 8px;
}
)");

  set_style();

  hesiod::ModelConfig model_config;

  std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry = registerDataModels(
      model_config);
  QtNodes::DataFlowGraphModel model(registry);

  model_config.set_shape({1024, 1024});

  QWidget main_widget;

  auto   menu_bar = new QMenuBar();
  QMenu *file = menu_bar->addMenu("File");
  auto   load_action = file->addAction("Load Scene");
  auto   save_action = file->addAction("Save Scene");
  file->addSeparator();
  auto quit_action = file->addAction("Quit");

  QVBoxLayout *l = new QVBoxLayout(&main_widget);

  l->addWidget(menu_bar);
  auto scene = new QtNodes::DataFlowGraphicsScene(model, &main_widget);

  auto view = new QtNodes::GraphicsView(scene);
  l->addWidget(view);
  l->setContentsMargins(0, 0, 0, 0);
  l->setSpacing(0);

  QObject::connect(save_action,
                   &QAction::triggered,
                   scene,
                   &QtNodes::DataFlowGraphicsScene::save);

  QObject::connect(load_action,
                   &QAction::triggered,
                   scene,
                   &QtNodes::DataFlowGraphicsScene::load);

  QObject::connect(scene,
                   &QtNodes::DataFlowGraphicsScene::sceneLoaded,
                   view,
                   &QtNodes::GraphicsView::centerScene);

  QObject::connect(scene,
                   &QtNodes::DataFlowGraphicsScene::nodeClicked,
                   [&scene]()
                   {
                     for (auto &nid : scene->selectedNodes())
                       std::cout << nid << "\n";
                   });

  QObject::connect(scene,
                   &QtNodes::DataFlowGraphicsScene::nodeContextMenu,
                   [&model](QtNodes::NodeId const node_id, QPointF const pos)
                   {
                     hesiod::BaseNode *p_node = model.delegateModel<hesiod::BaseNode>(
                         node_id);

                     p_node->context_menu(pos);
                   });

  QObject::connect(quit_action,
                   &QAction::triggered,
                   []()
                   {
                     LOG_DEBUG("quitting...");
                     QApplication::quit();
                   });

  main_widget.setWindowTitle("Hesiod");
  main_widget.showNormal();

  hesiod::HViewer2d *viewer2d = new hesiod::HViewer2d(&model_config, scene);
  viewer2d->show();

  // QWidget window;
  // window.setWindowTitle("Viewer 2D");
  // QVBoxLayout       *layout = new QVBoxLayout(&window);
  // hesiod::HViewer2d *viewer2d = new hesiod::HViewer2d(&model_config, scene);
  // layout->addWidget((QWidget *)viewer2d);
  // // QObject::connect(scene,
  // //                  &QtNodes::DataFlowGraphicsScene::nodeSelected,
  // //                  viewer2d,
  // //                  &hesiod::HViewer2d::update_viewport);
  // window.show();

  // // TODO find a way to empty the editor when there are no more nodes
  // // selected
  // QObject::connect(scene,
  //                  &QtNodes::DataFlowGraphicsScene::nodeSelected,
  //                  [&model, &scene, &settings_dialog]()
  //                  { settings_dialog.update_layout(model, scene->selectedNodes()); });

  return app.exec();
}
