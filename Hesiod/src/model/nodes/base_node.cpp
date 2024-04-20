/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/base_node.hpp"
#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

BaseNode::BaseNode(const ModelConfig *p_config) : p_config(p_config) {}

QtNodes::NodeDataType BaseNode::dataType(QtNodes::PortType  port_type,
                                         QtNodes::PortIndex port_index) const
{
  switch (port_type)
  {
  case QtNodes::PortType::In:
    return this->input_types[port_index];
  case QtNodes::PortType::Out:
    return this->output_types[port_index];
  default:
    throw std::runtime_error("wrong port type");
  }
}

QString BaseNode::name() const { return this->node_caption; }

unsigned int BaseNode::nPorts(QtNodes::PortType port_type) const
{
  switch (port_type)
  {
  case QtNodes::PortType::In:
    return this->input_types.size();
  case QtNodes::PortType::Out:
    return this->output_types.size();
  default:
    return 0;
  }
}

QString BaseNode::portCaption(QtNodes::PortType  port_type,
                              QtNodes::PortIndex port_index) const
{
  switch (port_type)
  {
  case QtNodes::PortType::In:
    return this->input_captions[port_index];
  case QtNodes::PortType::Out:
    return this->output_captions[port_index];
  default:
    return "??? unknown ???";
  }
}

bool BaseNode::portCaptionVisible(QtNodes::PortType /* port_type */,
                                  QtNodes::PortIndex /* port_index */) const
{
  return true;
}

// --- serialization

QJsonObject BaseNode::save() const
{
  QJsonObject model_json = QtNodes::NodeDelegateModel::save();

  for (auto &[key, attr] : this->attr)
  {
    QString qkey = QString::fromStdString(key);
    model_json[qkey] = attr->save();
  }
  return model_json;
}

void BaseNode::load(QJsonObject const &p)
{
  for (auto &[key, attr] : this->attr)
  {
    QString qkey = QString::fromStdString(key);
    attr->load(p[qkey].toObject());
  }

  // TODO overkill? but necessary since we do not serialize the heightmap fields
  // themselves so that they need to be recomputed after reloading the node internal
  // state (besides the heightmaps)
  this->compute();

  Q_EMIT this->node_loaded();
}

// --- debugging

void BaseNode::log_debug()
{
  LOG_DEBUG("caption: %s", this->caption().toStdString().c_str());
  LOG_DEBUG("name: %s", this->name().toStdString().c_str());
  LOG_DEBUG("address: %s", std::to_string((unsigned long long)(void **)this).c_str());

  for (QtNodes::PortType port_type :
       {QtNodes::PortType::In, QtNodes::PortType::Out, QtNodes::PortType::None})
    for (uint port_index = 0; port_index < this->nPorts(port_type); port_index++)
    {
      std::string c = this->portCaption(port_type, port_index).toStdString();
      std::string t = this->dataType(port_type, port_index).name.toStdString();
      LOG_DEBUG("port: {%d, %d, %s, %s}",
                (int)port_type,
                port_index,
                t.c_str(),
                c.c_str());
    }

  LOG_DEBUG("attributes");
  for (auto &[key, attr] : this->attr)
    attr->log_debug();
}

} // namespace hesiod
