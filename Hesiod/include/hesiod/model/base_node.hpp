/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QtCore/QObject>
#include <QtNodes/NodeDelegateModel>

#include "highmap/primitives.hpp"
#include "macrologger.h"

#include "hesiod/model/heightmap_data.hpp"
#include "hesiod/model/model_config.hpp"

#define DEFAULT_KW 2.f
#define DEFAULT_SEED 1

#define HSD_GET_POINTER(X) X.lock() ? X.lock()->get_ref() : nullptr

namespace hesiod
{

class BaseNode : public QtNodes::NodeDelegateModel
{
  Q_OBJECT

public:
  BaseNode(){};

  QString caption() const override { return this->node_caption; }

  bool captionVisible() const override { return true; }

  QtNodes::NodeDataType dataType(QtNodes::PortType  port_type,
                                 QtNodes::PortIndex port_index) const override
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

  QString name() const override { return this->node_caption; }

  unsigned int nPorts(QtNodes::PortType port_type) const override
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

  QString portCaption(QtNodes::PortType  port_type,
                      QtNodes::PortIndex port_index) const override
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

  bool portCaptionVisible(QtNodes::PortType /* port_type */,
                          QtNodes::PortIndex /* port_index */) const override
  {
    return true;
  }

protected:
  std::vector<QtNodes::NodeDataType> input_types = {};
  std::vector<QtNodes::NodeDataType> output_types = {};

  std::vector<QString> input_captions = {};
  std::vector<QString> output_captions = {};

  QString node_caption = "??? undefined ???";
};

// helpers
bool convert_qjsonvalue_to_float(QJsonValue v, float &out);
bool convert_qjsonvalue_to_int(QJsonValue v, int &out);
bool convert_qjsonvalue_to_uint(QJsonValue v, uint &out);

} // namespace hesiod