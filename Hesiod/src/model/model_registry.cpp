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
  LOG_DEBUG("initializing node registry...");

  auto ret = std::make_shared<QtNodes::NodeDelegateModelRegistry>();

  HSD_REGISTER_NODE(hesiod::Bump, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::Clamp, "Filter/Range");
  HSD_REGISTER_NODE(hesiod::Cloud, "Geometry/Cloud");
  HSD_REGISTER_NODE(hesiod::CloudToArrayInterp, "Primitive/Authoring");
  HSD_REGISTER_NODE(hesiod::Colorize, "Texture");
  HSD_REGISTER_NODE(hesiod::ColorizeSolid, "Texture");
  HSD_REGISTER_NODE(hesiod::Comment, "Comment");
  HSD_REGISTER_NODE(hesiod::DataPreview, "Debug");
  HSD_REGISTER_NODE(hesiod::Dilation, "Operator/Morphology");
  HSD_REGISTER_NODE(hesiod::ExpandShrink, "Filter/Recast");
  HSD_REGISTER_NODE(hesiod::ExportHeightmap, "IO/Files");
  HSD_REGISTER_NODE(hesiod::ExportTexture, "IO/Files");
  HSD_REGISTER_NODE(hesiod::Fold, "Filter/Recast");
  HSD_REGISTER_NODE(hesiod::Gain, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::GammaCorrection, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::Gradient, "Math/Gradient");
  HSD_REGISTER_NODE(hesiod::GradientAngle, "Math/Gradient");
  HSD_REGISTER_NODE(hesiod::GradientNorm, "Math/Gradient");
  HSD_REGISTER_NODE(hesiod::GradientTalus, "Math/Gradient");
  HSD_REGISTER_NODE(hesiod::HeightMapToMask, "Converter");
  HSD_REGISTER_NODE(hesiod::Laplace, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::Lerp, "Operator/Blend");
  HSD_REGISTER_NODE(hesiod::Median3x3, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::MixTexture, "Texture");
  HSD_REGISTER_NODE(hesiod::Noise, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseFbm, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseIq, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseJordan, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoisePingpong, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseRidged, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseSwiss, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::Plateau, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::Remap, "Filter/Range");
  HSD_REGISTER_NODE(hesiod::Rescale, "Filter/Range");
  HSD_REGISTER_NODE(hesiod::SetAlpha, "Texture");
  HSD_REGISTER_NODE(hesiod::SmoothCpulse, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::SmoothFill, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::ZeroedEdges, "Math/Boundaries");

  return ret;
}

} // namespace hesiod
