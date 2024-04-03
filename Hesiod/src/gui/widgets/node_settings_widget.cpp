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
                                       QWidget                        *parent)
    : p_scene(p_scene), parent(parent)
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

  // build up layout (actually empty at this point)
  this->setWindowTitle("Node settings");

  this->layout = new QVBoxLayout(this);
  this->layout->setSpacing(0);

  // pin this node
  {
    this->checkbox_pin_node = new QCheckBox("Pin current node");
    this->checkbox_pin_node->setChecked(false);
    this->layout->addWidget(this->checkbox_pin_node);
  }

  this->setLayout(this->layout);
}

void NodeSettingsWidget::on_node_deleted(QtNodes::NodeId const node_id)
{
  if (this->current_node_id == node_id)
  {
    clear_layout(this->layout);
    this->current_node_id = -1;
  }
}

void NodeSettingsWidget::update_layout(QtNodes::NodeId const node_id)
{
  if (!this->checkbox_pin_node->isChecked())
  {
    this->current_node_id = node_id;
    hesiod::BaseNode *p_node = this->p_model->delegateModel<hesiod::BaseNode>(node_id);

    clear_layout(this->layout);

    AttributesWidget *attributes_widget = new AttributesWidget(&p_node->attr,
                                                               &p_node->attr_ordered_key);

    connect(attributes_widget,
            &AttributesWidget::value_changed,
            [p_node]() { p_node->compute(); });

    // this->layout->addWidget(this->checkbox_pin_node);

    this->checkbox_pin_node = new QCheckBox("Pin current node");
    this->checkbox_pin_node->setChecked(false);
    this->layout->addWidget(this->checkbox_pin_node);

    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    this->layout->addWidget(line);

    this->layout->addWidget(attributes_widget);
    this->layout->addStretch();
  }
}

} // namespace hesiod
