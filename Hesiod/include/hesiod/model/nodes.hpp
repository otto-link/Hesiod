/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QtCore/QObject>
#include <QtNodes/NodeDelegateModel>

#include "highmap/primitives.hpp"
#include "macrologger.h"

#include "hesiod/data/heightmap_data.hpp"
#include "hesiod/data/mask_data.hpp"
#include "hesiod/model/base_node.hpp"
#include "hesiod/model/enum_mapping.hpp"

namespace hesiod
{

/**
 * @brief GammaCorrection class.
 */
class GammaCorrection : public BaseNode
{
public:
  GammaCorrection(const ModelConfig &config);

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(
      QtNodes::PortIndex /* port_index */) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::weak_ptr<MaskData>        mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief HeightMapToMask class, to convert a heightmap to a class.
 */
class HeightMapToMask : public BaseNode
{
public:
  HeightMapToMask(const ModelConfig &config);

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(
      QtNodes::PortIndex /* port_index */) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData> in;
  std::shared_ptr<MaskData>    mask;
};

/**
 * @brief Noise class, noise primitive generation.
 */
class Noise : public BaseNode
{

public:
  /**
   * @brief Constructor.
   * @param config Noise graph configuration (shape, tiling...).
   */
  Noise(const ModelConfig &config);

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

/**
 * @brief NoiseFbm class, noise primitive generation.
 */
class NoiseFbm : public BaseNode
{
public:
  NoiseFbm(const ModelConfig &config);

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(
      QtNodes::PortIndex /* port_index */) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   dx, dy, envelope;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief SmoothCpulse class.
 */
class SmoothCpulse : public BaseNode
{
public:
  SmoothCpulse(const ModelConfig &config);

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(
      QtNodes::PortIndex /* port_index */) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::weak_ptr<MaskData>        mask;
  std::shared_ptr<HeightMapData> out;
};

} // namespace hesiod
