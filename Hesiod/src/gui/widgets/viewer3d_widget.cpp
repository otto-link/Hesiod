/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <set>

#include <QCheckBox>
#include <QComboBox>
#include <QTimer>

#include "highmap/heightmap.hpp"
#include "highmap/io.hpp"

#include "hesiod/data/heightmap_data.hpp"
#include "hesiod/gui/viewer3d_widget.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/graph_model_addon.hpp"

namespace hesiod
{

Viewer3dWidget::Viewer3dWidget(ModelConfig                    *p_config,
                               QtNodes::DataFlowGraphicsScene *p_scene,
                               QWidget                        *parent,
                               std::string                     label)
    : p_config(p_config), p_scene(p_scene), parent(parent)
{
  this->p_model = (HsdDataFlowGraphModel *)&this->p_scene->graphModel();

  // update connection
  QObject::connect(this->p_scene,
                   &QtNodes::DataFlowGraphicsScene::nodeSelected,
                   this,
                   &hesiod::Viewer3dWidget::on_node_selected);

  QObject::connect(this->p_model,
                   &hesiod::HsdDataFlowGraphModel::nodeDeleted,
                   this,
                   &hesiod::Viewer3dWidget::on_node_deleted);

  QObject::connect(this->p_model,
                   &HsdDataFlowGraphModel::computingFinished,
                   this,
                   &hesiod::Viewer3dWidget::update_after_computing);

  // --- build up layout
  this->setWindowTitle("Viewer 3D");
  this->setMinimumSize(256, 256);
  this->setStyleSheet("border: 1px");

  QGridLayout *layout = new QGridLayout(this);

  // title
  if (label != "")
  {
    QLabel *widget = new QLabel(label.c_str());
    QFont   f = widget->font();
    f.setBold(true);
    widget->setFont(f);
    layout->addWidget(widget, 0, 0);
  }

  // pin this node
  {
    this->checkbox_pin_node = new QCheckBox("Pin current node");
    this->checkbox_pin_node->setChecked(false);
    layout->addWidget(this->checkbox_pin_node, 0, 1);
  }

  // show color
  {
    this->checkbox_texture = new QCheckBox("Render texture");
    this->checkbox_texture->setChecked(true);
    layout->addWidget(this->checkbox_texture, 0, 2);
  }

  {
    this->checkbox_mask = new QCheckBox("Render mask");
    this->checkbox_mask->setChecked(true);
    layout->addWidget(this->checkbox_mask, 0, 3);
  }

  // openGL widget
  {
    this->gl_viewer = new HmapGLViewer(this->p_config, nullptr);
    layout->addWidget(this->gl_viewer, 1, 0, 1, 4);
  }

  this->setLayout(layout);

  connect(this->checkbox_texture,
          &QCheckBox::stateChanged,
          [this]()
          {
            this->gl_viewer->render_texture = this->checkbox_texture->checkState();
            this->update_viewport();
          });

  connect(this->checkbox_mask,
          &QCheckBox::stateChanged,
          [this]()
          {
            this->gl_viewer->render_mask = this->checkbox_mask->checkState();
            this->update_viewport();
          });
}

void Viewer3dWidget::on_node_deleted(QtNodes::NodeId const node_id)
{
  if (this->current_node_id == node_id)
    this->reset();
}

void Viewer3dWidget::on_node_selected(QtNodes::NodeId const node_id)
{
  if (node_id != this->current_node_id)
    this->update_viewport(node_id);
}

void Viewer3dWidget::reset()
{
  this->current_node_id = std::numeric_limits<uint>::max();
  this->checkbox_pin_node->setChecked(false);
  this->checkbox_texture->setChecked(true);
  this->checkbox_mask->setChecked(true);
  this->gl_viewer->reset();
};

void Viewer3dWidget::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);
  Q_EMIT this->resized(event->size().width(), event->size().height());
  this->update_viewport();
}

void Viewer3dWidget::update_after_computing(QtNodes::NodeId const node_id)
{
  if (node_id == this->current_node_id)
    this->update_viewport(node_id);
}

void Viewer3dWidget::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);
  this->update_viewport();
}

void Viewer3dWidget::update_viewport(QtNodes::NodeId const node_id)
{
  if (!this->checkbox_pin_node->isChecked())
    this->current_node_id = node_id;

  QtNodes::NodeData *p_data = nullptr;
  QtNodes::NodeData *p_color = nullptr;

  if (this->p_model->allNodeIds().contains(this->current_node_id))
  {
    hesiod::BaseNode *p_node = this->p_model->delegateModel<hesiod::BaseNode>(
        this->current_node_id);
    p_data = p_node->get_viewer3d_data();
    p_color = p_node->get_viewer3d_color();
  }

  if (this->isVisible())
    this->gl_viewer->set_data(p_data, p_color);
}

void Viewer3dWidget::update_viewport() { this->update_viewport(this->current_node_id); }

} // namespace hesiod
