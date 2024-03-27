/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QtCore/QObject>
#include <QtNodes/NodeDelegateModel>

#include "highmap/primitives.hpp"
#include "macrologger.h"

#include "hesiod/model/base_node.hpp"
#include "hesiod/model/enum_mapping.hpp"

namespace hesiod
{

/**
 * @brief Noise class, noise primitive generation.
 */
class Noise : public BaseNode
{

public:
  /**
   * @brief Constructor.
   * @param config Noise graph configuration (shape, tiling...).
   * @param update_after_ctor Whether should be updated or not at the end of the
   * constructor. It used and set to false when a node (like a view node) is derived from
   * this class, to avoid updating two times the same kind of node.
   */
  Noise(const ModelConfig &config, const bool update_after_ctor = false);

  /**
   * @brief Node computation.
   */
  void compute() override;

  /**
   * @brief Return a reference to the output data for a given output port.
   * @return Shared pointer referencing the output data (as a generic NodeData).
   */
  std::shared_ptr<QtNodes::NodeData> outData(
      QtNodes::PortIndex /* port_index */) override;

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