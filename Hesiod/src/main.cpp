/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
typedef unsigned int uint;
#include <iostream>

#include <QApplication>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QWidget>

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModelRegistry>

#include "highmap/vector.hpp"
#include "macrologger.h"

#include "hesiod/model/heightmap_data.hpp"
#include "hesiod/model/model_config.hpp"
#include "hesiod/model/noise.hpp"

static std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registerDataModels(
    hesiod::ModelConfig &config)
{
  auto ret = std::make_shared<QtNodes::NodeDelegateModelRegistry>();
  ret->registerModel<hesiod::Noise>([&config]()
                                    { return std::make_unique<hesiod::Noise>(config); },
                                    "Primitives");
  return ret;
}

int main(int argc, char *argv[])
{
  hesiod::ModelConfig model_config;

  std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry = registerDataModels(
      model_config);
  QtNodes::DataFlowGraphModel data_flow_graph_model(registry);

  QApplication app(argc, argv);

  model_config.shape = {1024, 1024};

  QWidget main_widget;

  auto   menu_bar = new QMenuBar();
  QMenu *menu = menu_bar->addMenu("File");
  auto   save_action = menu->addAction("Save Scene");
  auto   load_action = menu->addAction("Load Scene");

  QVBoxLayout *l = new QVBoxLayout(&main_widget);

  l->addWidget(menu_bar);
  auto scene = new QtNodes::DataFlowGraphicsScene(data_flow_graph_model, &main_widget);

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

  main_widget.setWindowTitle("Hesiod");
  main_widget.showNormal();

  return app.exec();
}
