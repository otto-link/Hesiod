/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QSpacerItem>

#include "macrologger.h"

#include "hesiod/gui/gui_utils.hpp"
#include "hesiod/gui/node_settings_widget.hpp"
#include "hesiod/model/graph_model_addon.hpp"

namespace hesiod
{

NodeSettingsWidget::NodeSettingsWidget(QtNodes::DataFlowGraphicsScene *p_scene,
                                       QWidget                        *parent,
                                       std::string                     label)
    : p_scene(p_scene), parent(parent), label(label)
{
  this->p_model = (HsdDataFlowGraphModel *)&this->p_scene->graphModel();

  // update connection
  QObject::connect(this->p_scene,
                   &QtNodes::DataFlowGraphicsScene::nodeSelected,
                   this,
                   &hesiod::NodeSettingsWidget::update_layout);

  QObject::connect(this->p_model,
                   &hesiod::HsdDataFlowGraphModel::nodeDeleted,
                   this,
                   &hesiod::NodeSettingsWidget::on_node_deleted);

  this->setWindowTitle("Node settings");
  this->setMinimumWidth(300);

  // build up layout
  this->layout = new QVBoxLayout(this);
  this->setLayout(this->layout);

  this->build_layout(this->current_node_id);
}

void NodeSettingsWidget::build_layout(QtNodes::NodeId const node_id)
{
  if (this->label != "")
  {
    QLabel *widget = new QLabel(this->label.c_str());
    QFont   f = widget->font();
    f.setBold(true);
    widget->setFont(f);
    this->layout->addWidget(widget);
  }

  this->checkbox_pin_node = new QCheckBox("Pin current node");
  this->checkbox_pin_node->setChecked(this->do_pin_node);
  this->layout->addWidget(this->checkbox_pin_node);

  if (!this->do_pin_node)
    this->current_node_id = node_id;

  if (this->p_model->allNodeIds().contains(this->current_node_id))
  {
    hesiod::BaseNode *p_node = this->p_model->delegateModel<hesiod::BaseNode>(
        this->current_node_id);

    AttributesWidget *attributes_widget = new AttributesWidget(&p_node->attr,
                                                               &p_node->attr_ordered_key);

    connect(attributes_widget,
            &AttributesWidget::value_changed,
            [p_node]() { p_node->compute(); });

    this->layout->addWidget(attributes_widget);
  }
  else
  {
    QLabel *widget = new QLabel("nothing to show");
    this->layout->addWidget(widget);
  }
}

void NodeSettingsWidget::on_node_deleted(QtNodes::NodeId const node_id)
{
  if (this->current_node_id == node_id)
  {
    this->current_node_id = -1;
    this->do_pin_node = false;
    clear_layout(this->layout);
    this->build_layout(this->current_node_id);
  }
}

void NodeSettingsWidget::update_layout(QtNodes::NodeId const node_id)
{
  this->do_pin_node = this->checkbox_pin_node == nullptr
                          ? false
                          : this->checkbox_pin_node->isChecked();
  if (!this->do_pin_node)
  {
    clear_layout(this->layout);
    this->build_layout(node_id);
  }
}

} // namespace hesiod
