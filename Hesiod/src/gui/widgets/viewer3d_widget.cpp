/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <set>

#include <QCheckBox>
#include <QComboBox>

#include "highmap/heightmap.hpp"
#include "highmap/io.hpp"
#include "macrologger.h"

#include "hesiod/data/heightmap_data.hpp"
#include "hesiod/data/mask_data.hpp"
#include "hesiod/gui/viewer3d_widget.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/graph_model_addon.hpp"

namespace hesiod
{

Viewer3dWidget::Viewer3dWidget(ModelConfig                    *p_config,
                               QtNodes::DataFlowGraphicsScene *p_scene,
                               QWidget                        *parent)
    : p_config(p_config), p_scene(p_scene), parent(parent)
{
  this->p_model = (HsdDataFlowGraphModel *)&this->p_scene->graphModel();

  // update connection
  QObject::connect(this->p_scene,
                   &QtNodes::DataFlowGraphicsScene::nodeSelected,
                   this,
                   &hesiod::Viewer3dWidget::on_node_selected);

  QObject::connect(this->p_model,
                   &HsdDataFlowGraphModel::computingFinished,
                   this,
                   &hesiod::Viewer3dWidget::update_after_computing);

  // --- build up layout
  this->setWindowTitle("Viewer 3D");
  this->setMinimumSize(512, 512);

  QGridLayout *layout = new QGridLayout(this);

  // pin this node
  {
    this->checkbox_pin_node = new QCheckBox("Pin current node");
    this->checkbox_pin_node->setChecked(false);
    layout->addWidget(this->checkbox_pin_node, 0, 0);
  }

  // openGL widget
  {
    this->gl_viewer = new HmapGLViewer(this->p_config, nullptr);
    layout->addWidget(this->gl_viewer, 1, 0);
  }

  this->setLayout(layout);
}

void Viewer3dWidget::on_node_selected(QtNodes::NodeId const node_id)
{
  if (node_id != this->current_node_id)
    this->update_viewport(node_id);
}

void Viewer3dWidget::reset()
{
  this->current_node_id = -1;
  this->checkbox_pin_node->setChecked(false);
  this->gl_viewer->reset();
};

void Viewer3dWidget::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);
  Q_EMIT this->resized(event->size().width(), event->size().height());
}

void Viewer3dWidget::update_after_computing(QtNodes::NodeId const node_id)
{
  if (node_id == this->current_node_id)

    this->update_viewport(node_id);
}

void Viewer3dWidget::update_viewport(QtNodes::NodeId const node_id)
{
  if (!this->checkbox_pin_node->isChecked())
    this->current_node_id = node_id;

  if (this->isVisible() && this->p_model->allNodeIds().contains(this->current_node_id))
  {
    hesiod::BaseNode *p_node = this->p_model->delegateModel<hesiod::BaseNode>(
        this->current_node_id);
    this->gl_viewer->set_data(p_node->get_viewer3d_data(), p_node->get_viewer3d_color());
  }
}

void Viewer3dWidget::update_viewport() { this->update_viewport(this->current_node_id); }

} // namespace hesiod
