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

NodeEditorWidget::NodeEditorWidget(hesiod::ModelConfig *p_model_config,
                                   QWidget * /* parent */)
    : p_model_config(p_model_config)
{
  // initialize the registry with a small data shape to avoid using excessive memory at
  // this stage
  hmap::Vec2<int> shape_bckp = this->p_model_config->shape;
  this->p_model_config->set_shape(hmap::Vec2<int>(8, 8));

  std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry = register_data_models(
      this->p_model_config);

  this->p_model_config->set_shape(shape_bckp);
  this->p_model_config->tiling = {4, 4};
  this->model = std::make_unique<hesiod::HsdDataFlowGraphModel>(registry,
                                                                this->p_model_config);

  // build layout
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  // --- QtNode editor widget

  this->scene = std::make_unique<QtNodes::DataFlowGraphicsScene>(*this->model.get());
  auto view = new QtNodes::GraphicsView(scene.get());

  layout->addWidget(view);
  this->setLayout(layout);

  // --- connections

  connect(this->get_scene_ref(),
          &QtNodes::DataFlowGraphicsScene::sceneLoaded,
          view,
          &QtNodes::GraphicsView::centerScene);

  connect(
      this->get_scene_ref(),
      &QtNodes::DataFlowGraphicsScene::nodeContextMenu,
      [this](QtNodes::NodeId const node_id, QPointF const pos)
      {
        hesiod::BaseNode *p_node = this->get_model_ref()->delegateModel<hesiod::BaseNode>(
            node_id);
        p_node->context_menu(pos);
      });

  // pass-through for the node updates
  connect(this->get_model_ref(),
          &HsdDataFlowGraphModel::computingStarted,
          this,
          &NodeEditorWidget::computingStarted);

  connect(this->get_model_ref(),
          &HsdDataFlowGraphModel::computingFinished,
          this,
          &NodeEditorWidget::computingFinished);
}

void NodeEditorWidget::load() { this->get_scene_ref()->load(); }

void NodeEditorWidget::save() { this->get_scene_ref()->save(); }

} // namespace hesiod
