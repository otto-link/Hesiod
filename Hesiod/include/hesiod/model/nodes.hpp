/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QtCore/QObject>
#include <QtNodes/NodeDelegateModel>

#include "highmap/primitives.hpp"
#include "macrologger.h"

#include "hesiod/data/cloud_data.hpp"
#include "hesiod/data/heightmap_data.hpp"
#include "hesiod/data/heightmap_rgba_data.hpp"
#include "hesiod/data/mask_data.hpp"
#include "hesiod/model/base_node.hpp"
#include "hesiod/model/enum_mapping.hpp"

namespace hesiod
{

/**
 * @brief Cloud class.
 */
class Cloud : public BaseNode
{
public:
  Cloud(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex port_index);

protected:
  std::shared_ptr<CloudData> out;
};

/**
 * @brief CloudToArrayInterp class.
 */
class CloudToArrayInterp : public BaseNode
{
public:
  CloudToArrayInterp(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->in.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex port_index);

protected:
  std::weak_ptr<CloudData>       in;
  std::weak_ptr<HeightMapData>   dx, dy;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Colorize class, colorize texture using an input field and a colormap.
 */
class Colorize : public BaseNode
{
public:
  Colorize(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->level.lock().get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->out.get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>       level, alpha;
  std::shared_ptr<HeightMapRGBAData> out;
};

/**
 * @brief ColorizeSolid class, colorize texture using a uniform solid color.
 */
class ColorizeSolid : public BaseNode
{
public:
  ColorizeSolid(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::shared_ptr<HeightMapRGBAData> out;
};

/**
 * @brief DataPreview class.
 */
class DataPreview : public BaseNode
{
public:
  DataPreview(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->elevation.lock().get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->elevation.lock().get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->elevation.lock().get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->color.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>     elevation;
  std::weak_ptr<HeightMapRGBAData> color;
};

/**
 * @brief GammaCorrection class.
 */
class GammaCorrection : public BaseNode
{
public:
  GammaCorrection(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

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
  HeightMapToMask(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->mask.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->mask.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

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

/**
 * @brief NoiseFbm class, noise primitive generation.
 */
class NoiseFbm : public BaseNode
{
public:
  NoiseFbm(const ModelConfig *p_config);

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

/**
 * @brief SmoothCpulse class.
 */
class SmoothCpulse : public BaseNode
{
public:
  SmoothCpulse(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::weak_ptr<MaskData>        mask;
  std::shared_ptr<HeightMapData> out;
};

} // namespace hesiod
