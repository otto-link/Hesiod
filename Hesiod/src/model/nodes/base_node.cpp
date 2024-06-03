/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include <QJsonObject>

#include "hesiod/gui/widgets.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/base_node.hpp"

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

void BaseNode::full_description_to_file(std::string filename)
{
  std::string desc = this->get_full_description();

  std::fstream f;
  f.open(filename, std::ios::out);
  f << desc;
  f.close();
}

QJsonObject BaseNode::full_description_to_json()
{
  QJsonObject json;

  json["description"] = QString::fromStdString(this->description);

  // inputs
  QJsonArray json_inputs;
  for (size_t k = 0; k < this->input_descriptions.size(); k++)
  {
    QJsonObject json;
    json["caption"] = this->input_captions[k];
    json["type"] = this->input_types[k].name;
    json["description"] = this->input_descriptions[k].c_str();
    json_inputs.push_back(json);
  }
  json["inputs"] = json_inputs;

  // outpus
  QJsonArray json_outputs;
  for (size_t k = 0; k < this->output_descriptions.size(); k++)
  {
    QJsonObject json;
    json["caption"] = this->output_captions[k];
    json["type"] = this->output_types[k].name;
    json["description"] = this->output_descriptions[k].c_str();
    json_outputs.push_back(json);
  }
  json["outputs"] = json_outputs;

  // attributes
  QJsonArray json_attr;

  for (auto &[key, str] : this->attribute_descriptions)
  {
    QJsonObject json;
    json["key"] = key.c_str();
    json["type"] = attribute_type_map.at(this->attr.at(key)->get_type()).c_str();

    // add available values if the attribute is an enumerate
    if (this->attr.at(key)->get_type() == hesiod::AttributeType::MAP_ENUM)
    {
      MapEnumAttribute *p_attr = this->attr.at(key)->get_ref<MapEnumAttribute>();

      if (!p_attr->value.empty())
      {
        str += " Available values: ";

        std::string last_choice = std::prev(p_attr->value.end())->first;

        for (auto &[choice, dummy] : p_attr->value)
          if (choice != last_choice)
            str += choice + ", ";
          else
            str += choice + '.';
      }
    }

    json["description"] = str.c_str();
    json_attr.push_back(json);
  }

  json["parameters"] = json_attr;

  return json;
}

std::string BaseNode::get_full_description()
{
  std::string desc = this->description;
  desc += "\n\n";

  desc += "Inputs\n";

  if (this->input_descriptions.size())
  {
    for (size_t k = 0; k < this->input_descriptions.size(); k++)
    {
      desc += " - '" + this->input_captions[k].toStdString() + "'";
      desc += " (" + this->input_types[k].name.toStdString() + "): ";
      desc += this->input_descriptions[k];
      desc += "\n";
    }
  }
  else
    desc += "None.";

  desc += "\n";
  desc += "Outputs\n";

  if (this->output_descriptions.size())
  {
    for (size_t k = 0; k < this->output_descriptions.size(); k++)
    {
      desc += " - '" + this->output_captions[k].toStdString() + "'";
      desc += " (" + this->output_types[k].name.toStdString() + "): ";
      desc += this->output_descriptions[k];
      desc += "\n";
    }
  }
  else
    desc += "None.";

  desc += "\n";
  desc += "Parameters\n";

  if (!this->attribute_descriptions.empty())
  {
    for (auto &[key, str] : this->attribute_descriptions)
    {
      desc += " - '" + key + "'";
      desc += " (" + attribute_type_map.at(this->attr.at(key)->get_type()) + "): ";
      desc += str;

      // add available values if the attribute is an enumerate
      if (this->attr.at(key)->get_type() == hesiod::AttributeType::MAP_ENUM)
      {
        MapEnumAttribute *p_attr = this->attr.at(key)->get_ref<MapEnumAttribute>();

        if (!p_attr->value.empty())
        {
          desc += " Available values: ";

          std::string last_choice = std::prev(p_attr->value.end())->first;

          for (auto &[choice, dummy] : p_attr->value)
            if (choice != last_choice)
              desc += choice + ", ";
            else
              desc += choice + '.';
        }
      }

      desc += "\n";
    }
  }
  else
    desc += "None.";

  desc += "\n";

  return desc;
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

void BaseNode::trigger_outputs_updated()
{
  for (uint k = 0; k < this->output_types.size(); k++)
  {
    QtNodes::PortIndex out_port_index = (QtNodes::PortIndex)k;
    Q_EMIT this->dataUpdated(out_port_index);
  }
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
  LOG->trace("caption: %s", this->caption().toStdString().c_str());
  LOG->trace("name: %s", this->name().toStdString().c_str());
  LOG->trace("address: %s", std::to_string((unsigned long long)(void **)this).c_str());

  for (QtNodes::PortType port_type :
       {QtNodes::PortType::In, QtNodes::PortType::Out, QtNodes::PortType::None})
    for (uint port_index = 0; port_index < this->nPorts(port_type); port_index++)
    {
      std::string c = this->portCaption(port_type, port_index).toStdString();
      std::string t = this->dataType(port_type, port_index).name.toStdString();
      LOG->trace("port: {{{}, {}, {}, {}}}",
                 (int)port_type,
                 port_index,
                 t.c_str(),
                 c.c_str());
    }

  LOG->trace("attributes");
  for (auto &[key, attr] : this->attr)
    attr->log_debug();
}

} // namespace hesiod
