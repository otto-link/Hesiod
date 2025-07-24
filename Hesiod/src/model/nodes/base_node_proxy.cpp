/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QVBoxLayout>

#include "gnode/graph.hpp"

#include "attributes/widgets/attributes_widget.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

std::string BaseNode::get_caption() const { return this->get_label(); }

void *BaseNode::get_data_ref(int port_index)
{
  return this->get_value_ref_void(port_index);
}

std::string BaseNode::get_data_type(int port_index) const
{
  return gnode::Node::get_data_type(port_index);
}

int BaseNode::get_nports() const { return gnode::Node::get_nports(); }

std::string BaseNode::get_port_caption(int port_index) const
{
  return gnode::Node::get_port_label(port_index);
};

gngui::PortType BaseNode::get_port_type(int port_index) const
{
  gnode::PortType ptype = gnode::Node::get_port_type(this->get_port_label(port_index));

  if (ptype == gnode::PortType::IN)
    return gngui::PortType::IN;
  else
    return gngui::PortType::OUT;
}

QWidget *BaseNode::get_qwidget_ref()
{
  QWidget *widget = new QWidget(this);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  widget->setLayout(layout);

  // add the data preview by default
  if (!this->data_preview)
  {
    this->data_preview = new DataPreview(this);
    layout->addWidget(this->data_preview);
  }

  // add specific content if any
  if (this->qwidget_fct)
    layout->addWidget(this->qwidget_fct(this));

  // add node settings widget
  if (false)
  {
    QLabel *label = new QLabel("Settings");
    layout->addWidget(label);

    bool        add_save_reset_state_buttons = false;
    std::string window_title = "";

    attr::AttributesWidget *attributes_widget = new attr::AttributesWidget(
        this->get_attr_ref(),
        this->get_attr_ordered_key_ref(),
        window_title,
        add_save_reset_state_buttons);

    QLayout *retrieved_layout = qobject_cast<QLayout *>(attributes_widget->layout());
    if (retrieved_layout)
    {
      retrieved_layout->setSpacing(6);
      retrieved_layout->setContentsMargins(8, 0, 8, 0);
    }

    layout->addWidget(attributes_widget);

    // connection(s)
    this->connect(attributes_widget,
                  &attr::AttributesWidget::value_changed,
                  [this]()
                  {
                    gnode::Graph *p_graph = this->get_p_graph();
                    p_graph->update(this->get_id());
                  });
  }

  return widget;
}

std::string BaseNode::get_tool_tip_text()
{
  std::string str = "ID: " + this->get_caption() + "/" + this->get_id();
  str += "\n\n";

  str += this->get_documentation_short();

  return str;
}

} // namespace hesiod
