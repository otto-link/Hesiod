/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/model_registry.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

std::shared_ptr<QtNodes::NodeDelegateModelRegistry> register_data_models(
    hesiod::ModelConfig *p_config)
{
  auto ret = std::make_shared<QtNodes::NodeDelegateModelRegistry>();

  HSD_REGISTER_NODE(hesiod::Cloud, "Geometry");
  HSD_REGISTER_NODE(hesiod::CloudToArrayInterp, "Geometry");
  HSD_REGISTER_NODE(hesiod::Colorize, "Texture");
  HSD_REGISTER_NODE(hesiod::ColorizeSolid, "Texture");
  HSD_REGISTER_NODE(hesiod::DataPreview, "Debug");
  HSD_REGISTER_NODE(hesiod::ExportHeightmap, "IO");
  HSD_REGISTER_NODE(hesiod::ExportTexture, "IO");
  HSD_REGISTER_NODE(hesiod::Gain, "Filter");
  HSD_REGISTER_NODE(hesiod::GammaCorrection, "Filter");
  HSD_REGISTER_NODE(hesiod::HeightMapToMask, "Converter");
  HSD_REGISTER_NODE(hesiod::Plateau, "Filter");
  HSD_REGISTER_NODE(hesiod::Noise, "Primitive");
  HSD_REGISTER_NODE(hesiod::NoiseFbm, "Primitive");
  HSD_REGISTER_NODE(hesiod::Remap, "Filter");
  HSD_REGISTER_NODE(hesiod::Rescale, "Filter");
  HSD_REGISTER_NODE(hesiod::SetAlpha, "Texture");
  HSD_REGISTER_NODE(hesiod::SmoothCpulse, "Filter");
  HSD_REGISTER_NODE(hesiod::SmoothFill, "Filter");
  HSD_REGISTER_NODE(hesiod::ZeroedEdges, "Math");

  return ret;
}

} // namespace hesiod