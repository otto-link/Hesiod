/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QPlainTextEdit>

#include <QtCore/QObject>
#include <QtNodes/NodeDelegateModel>

#include "hesiod/data/heightmap_data.hpp"
#include "hesiod/model/base_node.hpp"

namespace hesiod
{

class Lerp : public BaseNode
{
public:
  Lerp(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   a, b, t;
  std::shared_ptr<HeightMapData> out;
};

} // namespace hesiod