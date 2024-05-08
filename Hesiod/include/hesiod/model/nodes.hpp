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
#include "hesiod/data/path_data.hpp"
#include "hesiod/model/base_node.hpp"
#include "hesiod/model/enum_mapping.hpp"

namespace hesiod
{

/**
 * @brief Abs class.
 */
class Abs : public BaseNode
{
public:
  Abs(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief AbsSmooth class.
 */
class AbsSmooth : public BaseNode
{
public:
  AbsSmooth(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Blend class.
 */
class Blend : public BaseNode
{
public:
  Blend(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in1, in2;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Bump class.
 */
class Bump : public BaseNode
{
public:
  Bump(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   dx, dy;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Clamp class.
 */
class Clamp : public BaseNode
{
public:
  Clamp(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Closing class.
 */
class Closing : public BaseNode
{
public:
  Closing(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

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
 * @brief CloudSDF class.
 */
class CloudSDF : public BaseNode
{
public:
  CloudSDF(const ModelConfig *p_config);

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
 * @brief ColorizeCmap class.
 */
class ColorizeCmap : public BaseNode
{
public:
  ColorizeCmap(const ModelConfig *p_config);

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
 * @brief ColorizeGradient class.
 */
class ColorizeGradient : public BaseNode
{
public:
  ColorizeGradient(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

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
 * @brief CombineMask class.
 */
class CombineMask : public BaseNode
{
public:
  CombineMask(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in1, in2;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Comment class.
 */
class Comment : public BaseNode
{
public:
  Comment(const ModelConfig *p_config);

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

  virtual QWidget *embeddedWidget() override;

private:
  QLabel *label_comment = nullptr;
};

/**
 * @brief Cos class.
 */
class Cos : public BaseNode
{
public:
  Cos(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
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
 * @brief Dendry class, noise primitive generation.
 */
class Dendry : public BaseNode
{
public:
  Dendry(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   control_function, dx, dy, envelope;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief DepressionFilling class.
 */
class DepressionFilling : public BaseNode
{
public:
  DepressionFilling(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out, fill_map;
};

/**
 * @brief Dilation class.
 */
class Dilation : public BaseNode
{
public:
  Dilation(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Erosion class.
 */
class Erosion : public BaseNode
{
public:
  Erosion(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief ExpandShrink class.
 */
class ExpandShrink : public BaseNode
{
public:
  ExpandShrink(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief ExportAsset class.
 */
class ExportAsset : public BaseNode
{
public:
  ExportAsset(const ModelConfig *p_config);

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
 * @brief ExportHeightmap class.
 */
class ExportHeightmap : public BaseNode
{
public:
  ExportHeightmap(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer3d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData> in;
};

/**
 * @brief ExportNormalMap class.
 */
class ExportNormalMap : public BaseNode
{
public:
  ExportNormalMap(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer3d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData> in;
};

/**
 * @brief ExportTexture class.
 */
class ExportTexture : public BaseNode
{
public:
  ExportTexture(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer3d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapRGBAData> in;
};

/**
 * @brief Fold class.
 */
class Fold : public BaseNode
{
public:
  Fold(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Gain class.
 */
class Gain : public BaseNode
{
public:
  Gain(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief GaussianDecay class.
 */
class GaussianDecay : public BaseNode
{
public:
  GaussianDecay(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief GaussianPulse class.
 */
class GaussianPulse : public BaseNode
{
public:
  GaussianPulse(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   dr;
  std::shared_ptr<HeightMapData> out;
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
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief GammaCorrectionLocal class.
 */
class GammaCorrectionLocal : public BaseNode
{
public:
  GammaCorrectionLocal(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Gradient class.
 */
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

/**
 * @brief GradientAngle class.
 */
class GradientAngle : public BaseNode
{
public:
  GradientAngle(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief GradientNorm class.
 */
class GradientNorm : public BaseNode
{
public:
  GradientNorm(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief GradientTalus class.
 */
class GradientTalus : public BaseNode
{
public:
  GradientTalus(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief HeightmapToMask class, to convert a heightmap to a class.
 */
class HeightmapToMask : public BaseNode
{
public:
  HeightmapToMask(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->mask.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->mask.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> mask;
};

/**
 * @brief HeightmapToRGBA class.
 */
class HeightmapToRGBA : public BaseNode
{
public:
  HeightmapToRGBA(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>       r, g, b, a;
  std::shared_ptr<HeightMapRGBAData> out;
};

/**
 * @brief HydraulicParticle class.
 */
class HydraulicParticle : public BaseNode
{
public:
  HydraulicParticle(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, bedrock, moisture_map, mask;
  std::shared_ptr<HeightMapData> out, erosion_map, deposition_map;
};

/**
 * @brief HydraulicRidge class.
 */
class HydraulicRidge : public BaseNode
{
public:
  HydraulicRidge(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief HydraulicStream class.
 */
class HydraulicStream : public BaseNode
{
public:
  HydraulicStream(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, bedrock, moisture_map, mask;
  std::shared_ptr<HeightMapData> out, erosion_map;
};

/**
 * @brief HydraulicVpipes class.
 */
class HydraulicVpipes : public BaseNode
{
public:
  HydraulicVpipes(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, bedrock, moisture_map, mask;
  std::shared_ptr<HeightMapData> out, erosion_map, deposition_map;
};

/**
 * @brief ImportHeightmap class.
 */
class ImportHeightmap : public BaseNode
{
public:
  ImportHeightmap(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Inverse class.
 */
class Inverse : public BaseNode
{
public:
  Inverse(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Laplace class.
 */
class Laplace : public BaseNode
{
public:
  Laplace(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Lerp class.
 */
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

/**
 * @brief MakeBinary class.
 */
class MakeBinary : public BaseNode
{
public:
  MakeBinary(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->out.get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief MakePeriodic class.
 */
class MakePeriodic : public BaseNode
{
public:
  MakePeriodic(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief MakePeriodicStitching class.
 */
class MakePeriodicStitching : public BaseNode
{
public:
  MakePeriodicStitching(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Median3x3 class.
 */
class Median3x3 : public BaseNode
{
public:
  Median3x3(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief MixTexture class.
 */
class MixTexture : public BaseNode
{
public:
  MixTexture(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapRGBAData>   in1, in2, in3, in4;
  std::shared_ptr<HeightMapRGBAData> out;
};

/**
 * @brief MorphologicalGradient class.
 */
class MorphologicalGradient : public BaseNode
{
public:
  MorphologicalGradient(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->out.get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
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
 * @brief NoiseIq class.
 */
class NoiseIq : public BaseNode
{
public:
  NoiseIq(const ModelConfig *p_config);

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
 * @brief NoiseJordan class.
 */
class NoiseJordan : public BaseNode
{
public:
  NoiseJordan(const ModelConfig *p_config);

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
 * @brief NoisePingpong class.
 */
class NoisePingpong : public BaseNode
{
public:
  NoisePingpong(const ModelConfig *p_config);

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
 * @brief NoiseRidged class.
 */
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

/**
 * @brief NoiseSwiss class.
 */
class NoiseSwiss : public BaseNode
{
public:
  NoiseSwiss(const ModelConfig *p_config);

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
 * @brief NormalDisplacement class.
 */
class NormalDisplacement : public BaseNode
{
public:
  NormalDisplacement(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Opening class.
 */
class Opening : public BaseNode
{
public:
  Opening(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Path class.
 */
class Path : public BaseNode
{
public:
  Path(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex port_index);

protected:
  std::shared_ptr<PathData> out;
};

/**
 * @brief PathBezier class.
 */
class PathBezier : public BaseNode
{
public:
  PathBezier(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex port_index);

protected:
  std::weak_ptr<PathData>   in;
  std::shared_ptr<PathData> out;
};

/**
 * @brief PathBezierRound class.
 */
class PathBezierRound : public BaseNode
{
public:
  PathBezierRound(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex port_index);

protected:
  std::weak_ptr<PathData>   in;
  std::shared_ptr<PathData> out;
};

/**
 * @brief PathBspline class.
 */
class PathBspline : public BaseNode
{
public:
  PathBspline(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex port_index);

protected:
  std::weak_ptr<PathData>   in;
  std::shared_ptr<PathData> out;
};

/**
 * @brief PathFractalize class.
 */
class PathFractalize : public BaseNode
{
public:
  PathFractalize(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex port_index);

protected:
  std::weak_ptr<PathData>   in;
  std::shared_ptr<PathData> out;
};

/**
 * @brief PathSDF class.
 */
class PathSDF : public BaseNode
{
public:
  PathSDF(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->in.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex port_index);

protected:
  std::weak_ptr<PathData>        in;
  std::weak_ptr<HeightMapData>   dx, dy;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief PathToHeightmap class.
 */
class PathToHeightmap : public BaseNode
{
public:
  PathToHeightmap(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->in.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex port_index);

protected:
  std::weak_ptr<PathData>        in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Plateau class.
 */
class Plateau : public BaseNode
{
public:
  Plateau(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief QuiltingExpand class.
 */
class QuiltingExpand : public BaseNode
{
public:
  QuiltingExpand(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief QuiltingShuffle class.
 */
class QuiltingShuffle : public BaseNode
{
public:
  QuiltingShuffle(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Remap class.
 */
class Remap : public BaseNode
{
public:
  Remap(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief RecastCliff class.
 */
class RecastCliff : public BaseNode
{
public:
  RecastCliff(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief RecurveKura class.
 */
class RecurveKura : public BaseNode
{
public:
  RecurveKura(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief RecurveS class.
 */
class RecurveS : public BaseNode
{
public:
  RecurveS(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief RelativeElevation class.
 */
class RelativeElevation : public BaseNode
{
public:
  RelativeElevation(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->out.get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Rescale class.
 */
class Rescale : public BaseNode
{
public:
  Rescale(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Rugosity class.
 */
class Rugosity : public BaseNode
{
public:
  Rugosity(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->out.get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Saturate class.
 */
class Saturate : public BaseNode
{
public:
  Saturate(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief ScanMask class.
 */
class ScanMask : public BaseNode
{
public:
  ScanMask(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief SelectCavities class.
 */
class SelectCavities : public BaseNode
{
public:
  SelectCavities(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->out.get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief SelectGt class.
 */
class SelectGt : public BaseNode
{
public:
  SelectGt(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->out.get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief SelectInterval class.
 */
class SelectInterval : public BaseNode
{
public:
  SelectInterval(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->out.get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief SelectPulse class.
 */
class SelectPulse : public BaseNode
{
public:
  SelectPulse(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->out.get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief SetAlpha class.
 */
class SetAlpha : public BaseNode
{
public:
  SetAlpha(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return nullptr; }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapRGBAData>   in;
  std::weak_ptr<HeightMapData>       alpha;
  std::shared_ptr<HeightMapRGBAData> out;
};

/**
 * @brief SharpenCone class.
 */
class SharpenCone : public BaseNode
{
public:
  SharpenCone(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, mask;
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
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief SmoothFill class.
 */
class SmoothFill : public BaseNode
{
public:
  SmoothFill(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out, deposition_map;
};

/**
 * @brief SmoothFillHoles class.
 */
class SmoothFillHoles : public BaseNode
{
public:
  SmoothFillHoles(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief SmoothFillSmearPeaks class.
 */
class SmoothFillSmearPeaks : public BaseNode
{
public:
  SmoothFillSmearPeaks(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Smoothstep3 class.
 */
class Smoothstep3 : public BaseNode
{
public:
  Smoothstep3(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Smoothstep5 class.
 */
class Smoothstep5 : public BaseNode
{
public:
  Smoothstep5(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Slope class.
 */
class Slope : public BaseNode
{
public:
  Slope(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   dr;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Step class.
 */
class Step : public BaseNode
{
public:
  Step(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   dr;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief Stratify class.
 */
class Stratify : public BaseNode
{
public:
  Stratify(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, noise, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief StratifyOblique class.
 */
class StratifyOblique : public BaseNode
{
public:
  StratifyOblique(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->mask.lock().get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, noise, mask;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief ValleyWidth class.
 */
class ValleyWidth : public BaseNode
{
public:
  ValleyWidth(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->in.lock().get(); }
  QtNodes::NodeData *get_viewer3d_color() { return this->out.get(); }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief WaveDune class.
 */
class WaveDune : public BaseNode
{
public:
  WaveDune(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   dr;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief WaveSine class.
 */
class WaveSine : public BaseNode
{
public:
  WaveSine(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   dr;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief WaveSquare class.
 */
class WaveSquare : public BaseNode
{
public:
  WaveSquare(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   dr;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief WaveTriangular class.
 */
class WaveTriangular : public BaseNode
{
public:
  WaveTriangular(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   dr;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief White class.
 */
class White : public BaseNode
{
public:
  White(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   envelope;
  std::shared_ptr<HeightMapData> out;
};

/**
 * @brief ZeroedEdges class.
 */
class ZeroedEdges : public BaseNode
{
public:
  ZeroedEdges(const ModelConfig *p_config);

  QtNodes::NodeData *get_preview_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer2d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_data() { return this->out.get(); }
  QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  void compute() override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port_index) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override;

protected:
  std::weak_ptr<HeightMapData>   in, dr;
  std::shared_ptr<HeightMapData> out;
};

} // namespace hesiod
