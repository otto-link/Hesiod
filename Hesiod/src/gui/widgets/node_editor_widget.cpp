/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QMenuBar>
#include <QVBoxLayout>

#include "hesiod/gui/node_editor_widget.hpp"
#include "hesiod/model/model_registry.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

NodeEditorWidget::NodeEditorWidget(hesiod::ModelConfig model_config,
                                   QWidget * /* parent */)
    : model_config(model_config)
{
  // initialize the registry with a small data shape to avoid using excessive memory at
  // this stage
  hmap::Vec2<int> shape_bckp = this->model_config.shape;
  this->model_config.set_shape(hmap::Vec2<int>(8, 8));

  std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry = register_data_models(
      this->model_config);

  this->model_config.set_shape(shape_bckp);
  this->model_config.tiling = {4, 4};
  this->model = std::make_unique<hesiod::HsdDataFlowGraphModel>(registry);

  // build layout
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  // --- menu bar

  // QMenuBar *menu_bar = new QMenuBar();
  // QMenu    *file = menu_bar->addMenu("File");
  // QAction  *load_action = file->addAction("Load Scene");
  // QAction  *save_action = file->addAction("Save Scene");

  // QMenu   *view_menu = menu_bar->addMenu("View");
  // QAction *view2d_action = view_menu->addAction("Add 2D view");
  // view2d_action->setCheckable(true);

  // layout->addWidget(menu_bar);

  // --- QtNode editor widget

  this->scene = std::make_unique<QtNodes::DataFlowGraphicsScene>(*this->model.get());
  auto view = new QtNodes::GraphicsView(scene.get());

  layout->addWidget(view);
  this->setLayout(layout);

  // viewer(s)
  this->viewer2d = std::make_unique<hesiod::Viewer2dWidget>(&this->model_config,
                                                            this->get_scene_ref());

  this->viewer3d = std::make_unique<hesiod::Viewer3dWidget>(&this->model_config,
                                                            this->get_scene_ref());

  this->viewer3d->show(); // fix me DBG

  // --- connections

  // QObject::connect(load_action,
  //                  &QAction::triggered,
  //                  this,
  //                  &NodeEditorWidget::load);

  // QObject::connect(save_action,
  //                  &QAction::triggered,
  //                  this,
  //                  &NodeEditorWidget::save);

  // QObject::connect(view2d_action,
  //                  &QAction::toggled,
  //                  [this, view2d_action]()
  //                  {
  //                    if (view2d_action->isChecked())
  //                      this->viewer2d.get()->show();
  //                    else
  //                      this->viewer2d.get()->hide();
  //                  });

  QObject::connect(this->get_scene_ref(),
                   &QtNodes::DataFlowGraphicsScene::sceneLoaded,
                   view,
                   &QtNodes::GraphicsView::centerScene);

  QObject::connect(
      this->get_scene_ref(),
      &QtNodes::DataFlowGraphicsScene::nodeContextMenu,
      [this](QtNodes::NodeId const node_id, QPointF const pos)
      {
        hesiod::BaseNode *p_node = this->get_model_ref()->delegateModel<hesiod::BaseNode>(
            node_id);
        p_node->context_menu(pos);
      });

  // pass-through for the node updates
  QObject::connect(this->get_model_ref(),
                   &HsdDataFlowGraphModel::computingStarted,
                   this,
                   &NodeEditorWidget::computingStarted);

  QObject::connect(this->get_model_ref(),
                   &HsdDataFlowGraphModel::computingFinished,
                   this,
                   &NodeEditorWidget::computingFinished);
}

} // namespace hesiod
