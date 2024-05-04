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

class Noise : public BaseNode
{

public:
  /**
   * @brief Constructor.
   * @param config Noise graph configuration (shape, tiling...).
   */
  Noise(const ModelConfig *p_config);

  /**
   * @brief Return reference to the data for each viewers.
   * @return Reference to the data.
   */
  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  /**
   * @brief Node computation.
   */
  void compute() override;

  /**
   * @brief Return a reference to the output data for a given output port.
   * @return Shared pointer referencing the output data (as a generic NodeData).
   */
  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  /**
   * @brief Method triggered when there are incoming data at a given input port.
   * @param data Incoming data (as a generic NodeData).
   * @param port_index Input port index.
   */
  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  /**
   * brief Reference to the input data.
   */
  std::weak_ptr<HeightMapData> dx, dy, envelope;

  /**
   * @brief Storage of the output data.
   */
  std::shared_ptr<HeightMapData> out;
};

} // namespace hesiod