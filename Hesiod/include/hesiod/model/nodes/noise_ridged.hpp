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

class NoiseRidged : public BaseNode
{
public:
  NoiseRidged(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   dx, dy, envelope;
  std::shared_ptr<HeightMapData> out;
};

} // namespace hesiod