/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QPlainTextEdit>

#include <QtCore/QObject>
#include <QtNodes/NodeDelegateModel>

#include "highmap/primitives.hpp"
#include "macrologger.h"

#include "hesiod/data/cloud_data.hpp"
#include "hesiod/data/heightmap_data.hpp"
#include "hesiod/data/heightmap_rgba_data.hpp"
#include "hesiod/model/base_node.hpp"
#include "hesiod/model/enum_mapping.hpp"

namespace hesiod
{

class Gradient : public BaseNode
{
public:
  Gradient(const ModelConfig *p_config);

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

  // specific preview GUI widget for this node
  virtual QWidget *embeddedWidget() override; // GUI

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> dx, dy;

private:
  PreviewVec2 *preview_vec2 = nullptr; // GUI
};

} // namespace hesiod