/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
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
  if (!this->data_preview)
    this->data_preview = new DataPreview(this);

  return (QWidget *)this->data_preview;
}

} // namespace hesiod
