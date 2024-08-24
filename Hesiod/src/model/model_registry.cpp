/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/model_registry.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

std::shared_ptr<QtNodes::NodeDelegateModelRegistry> register_data_models(
    hesiod::ModelConfig *p_config)
{
  auto log = hesiod::Logger::get_logger();

  log->debug("initializing node registry");

  auto ret = std::make_shared<QtNodes::NodeDelegateModelRegistry>();

  HSD_REGISTER_NODE(hesiod::Abs, "Math/Base");
  HSD_REGISTER_NODE(hesiod::AbsSmooth, "Math/Base");
  HSD_REGISTER_NODE(hesiod::AccumulationCurvature, "Features/Landform");
  HSD_REGISTER_NODE(hesiod::Blend, "Operator/Blend");
  HSD_REGISTER_NODE(hesiod::Bump, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::Caldera, "Primitive/Geological");
  HSD_REGISTER_NODE(hesiod::Clamp, "Filter/Range");
  HSD_REGISTER_NODE(hesiod::Closing, "Operator/Morphology");
  HSD_REGISTER_NODE(hesiod::Cloud, "Geometry/Cloud");
  HSD_REGISTER_NODE(hesiod::CloudLattice, "Geometry/Cloud");
  HSD_REGISTER_NODE(hesiod::CloudMerge, "Geometry/Cloud");
  HSD_REGISTER_NODE(hesiod::CloudRandom, "Geometry/Cloud");
  HSD_REGISTER_NODE(hesiod::CloudRemapValues, "Geometry/Cloud");
  HSD_REGISTER_NODE(hesiod::CloudSDF, "Geometry/Cloud");
  HSD_REGISTER_NODE(hesiod::CloudToArrayInterp, "Primitive/Authoring");
  HSD_REGISTER_NODE(hesiod::CloudToPath, "Converter");
  HSD_REGISTER_NODE(hesiod::ColorizeCmap, "Texture");
  HSD_REGISTER_NODE(hesiod::ColorizeGradient, "Texture");
  HSD_REGISTER_NODE(hesiod::ColorizeSolid, "Texture");
  HSD_REGISTER_NODE(hesiod::CombineMask, "Mask");
  HSD_REGISTER_NODE(hesiod::Comment, "Comment");
  HSD_REGISTER_NODE(hesiod::ConvolveSVD, "Math/Convolution");
  HSD_REGISTER_NODE(hesiod::Cos, "Math/Base");
  HSD_REGISTER_NODE(hesiod::Crater, "Primitive/Geological");
  HSD_REGISTER_NODE(hesiod::DataAnalysis, "Debug");
  HSD_REGISTER_NODE(hesiod::DataPreview, "Debug");
  HSD_REGISTER_NODE(hesiod::Dendry, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::DepressionFilling, "Erosion");
  HSD_REGISTER_NODE(hesiod::Detrend, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::Dilation, "Operator/Morphology");
  HSD_REGISTER_NODE(hesiod::DistanceTransform, "Operator/Morphology");
  HSD_REGISTER_NODE(hesiod::Erosion, "Operator/Morphology");
  HSD_REGISTER_NODE(hesiod::ExpandShrink, "Filter/Recast");
  HSD_REGISTER_NODE(hesiod::ExportAsset, "IO/Files");
  HSD_REGISTER_NODE(hesiod::ExportCloud, "IO/Files");
  HSD_REGISTER_NODE(hesiod::ExportHeightmap, "IO/Files");
  HSD_REGISTER_NODE(hesiod::ExportNormalMap, "IO/Files");
  HSD_REGISTER_NODE(hesiod::ExportPath, "IO/Files");
  HSD_REGISTER_NODE(hesiod::ExportTexture, "IO/Files");
  HSD_REGISTER_NODE(hesiod::Falloff, "Math/Boundaries");
  HSD_REGISTER_NODE(hesiod::FillTalus, "Operator/Transform");
  HSD_REGISTER_NODE(hesiod::Fold, "Filter/Recast");
  HSD_REGISTER_NODE(hesiod::Gain, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::GammaCorrection, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::GammaCorrectionLocal, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::GaussianDecay, "Math/Base");
  HSD_REGISTER_NODE(hesiod::GaussianPulse, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::Gradient, "Math/Gradient");
  HSD_REGISTER_NODE(hesiod::GradientAngle, "Math/Gradient");
  HSD_REGISTER_NODE(hesiod::GradientNorm, "Math/Gradient");
  HSD_REGISTER_NODE(hesiod::GradientTalus, "Math/Gradient");
  HSD_REGISTER_NODE(hesiod::HeightmapToKernel, "Converter");
  HSD_REGISTER_NODE(hesiod::HeightmapToMask, "Converter");
  HSD_REGISTER_NODE(hesiod::HeightmapToRGBA, "Converter");
  HSD_REGISTER_NODE(hesiod::HydraulicBlur, "Erosion/Hydraulic");
  HSD_REGISTER_NODE(hesiod::HydraulicParticle, "Erosion/Hydraulic");
  HSD_REGISTER_NODE(hesiod::HydraulicRidge, "Erosion/Hydraulic");
  HSD_REGISTER_NODE(hesiod::HydraulicStream, "Erosion/Hydraulic");
  HSD_REGISTER_NODE(hesiod::HydraulicVpipes, "Erosion/Hydraulic");
  HSD_REGISTER_NODE(hesiod::ImportHeightmap, "IO/Files");
  HSD_REGISTER_NODE(hesiod::Inverse, "Math/Base");
  HSD_REGISTER_NODE(hesiod::KmeansClustering2, "Features/Clustering");
  HSD_REGISTER_NODE(hesiod::KmeansClustering3, "Features/Clustering");
  HSD_REGISTER_NODE(hesiod::KernelGabor, "Primitive/Kernel");
  HSD_REGISTER_NODE(hesiod::KernelPrim, "Primitive/Kernel");
  HSD_REGISTER_NODE(hesiod::Laplace, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::Lerp, "Operator/Blend");
  HSD_REGISTER_NODE(hesiod::MakeBinary, "Operator/Morphology");
  HSD_REGISTER_NODE(hesiod::MakePeriodic, "Operator/Tiling");
  HSD_REGISTER_NODE(hesiod::MakePeriodicStitching, "Operator/Tiling");
  HSD_REGISTER_NODE(hesiod::Median3x3, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::Mixer, "Operator/Blend");
  HSD_REGISTER_NODE(hesiod::MixTexture, "Texture");
  HSD_REGISTER_NODE(hesiod::MorphologicalGradient, "Operator/Morphology");
  HSD_REGISTER_NODE(hesiod::Noise, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseFbm, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseIq, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseJordan, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoisePingpong, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseParberry, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseRidged, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseSwiss, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NormalDisplacement, "Filter/Recast");
  HSD_REGISTER_NODE(hesiod::Opening, "Operator/Morphology");
  HSD_REGISTER_NODE(hesiod::Paraboloid, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::Path, "Geometry/Path");
  HSD_REGISTER_NODE(hesiod::PathBezier, "Geometry/Path");
  HSD_REGISTER_NODE(hesiod::PathBezierRound, "Geometry/Path");
  HSD_REGISTER_NODE(hesiod::PathBspline, "Geometry/Path");
  HSD_REGISTER_NODE(hesiod::PathDecasteljau, "Geometry/Path");
  HSD_REGISTER_NODE(hesiod::PathFractalize, "Geometry/Path");
  HSD_REGISTER_NODE(hesiod::PathMeanderize, "Geometry/Path");
  HSD_REGISTER_NODE(hesiod::PathResample, "Geometry/Path");
  HSD_REGISTER_NODE(hesiod::PathSDF, "Geometry/Path");
  HSD_REGISTER_NODE(hesiod::PathToCloud, "Converter");
  HSD_REGISTER_NODE(hesiod::PathToHeightmap, "Geometry/Path");
  HSD_REGISTER_NODE(hesiod::Plateau, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::QuiltingExpand, "Operator/Expand");
  HSD_REGISTER_NODE(hesiod::QuiltingShuffle, "Operator/Resynthesis");
  HSD_REGISTER_NODE(hesiod::RadialDisplacementToXy, "Math");
  HSD_REGISTER_NODE(hesiod::Remap, "Filter/Range");
  HSD_REGISTER_NODE(hesiod::RecastCliff, "Filter/Recast");
  HSD_REGISTER_NODE(hesiod::RecastSag, "Filter/Recast");
  HSD_REGISTER_NODE(hesiod::RecurveKura, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::RecurveS, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::RelativeElevation, "Features/Landform");
  HSD_REGISTER_NODE(hesiod::Rescale, "Filter/Range");
  HSD_REGISTER_NODE(hesiod::ReverseMidpoint, "Primitive/Authoring");
  HSD_REGISTER_NODE(hesiod::Ridgelines, "Primitive/Authoring");
  HSD_REGISTER_NODE(hesiod::Rift, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::Rugosity, "Features");
  HSD_REGISTER_NODE(hesiod::Saturate, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::ScanMask, "Mask/Adjust");
  HSD_REGISTER_NODE(hesiod::SelectAngle, "Mask/Selector");
  HSD_REGISTER_NODE(hesiod::SelectBlobLog, "Mask/Selector");
  HSD_REGISTER_NODE(hesiod::SelectCavities, "Mask/Selector");
  HSD_REGISTER_NODE(hesiod::SelectElevationSlope, "Mask/Selector");
  HSD_REGISTER_NODE(hesiod::SelectGt, "Mask/Selector");
  HSD_REGISTER_NODE(hesiod::SelectInterval, "Mask/Selector");
  HSD_REGISTER_NODE(hesiod::SelectInwardOutward, "Mask/Selector");
  HSD_REGISTER_NODE(hesiod::SelectPulse, "Mask/Selector");
  HSD_REGISTER_NODE(hesiod::SelectRivers, "Mask/Selector");
  HSD_REGISTER_NODE(hesiod::SelectTransitions, "Mask/Selector");
  HSD_REGISTER_NODE(hesiod::SetAlpha, "Texture");
  HSD_REGISTER_NODE(hesiod::ShapeIndex, "Features/Landform");
  HSD_REGISTER_NODE(hesiod::SharpenCone, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::ShiftElevation, "Filter/Range");
  HSD_REGISTER_NODE(hesiod::SmoothCpulse, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::SmoothFill, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::SmoothFillHoles, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::SmoothFillSmearPeaks, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::Smoothstep3, "Math/Base");
  HSD_REGISTER_NODE(hesiod::Smoothstep5, "Math/Base");
  HSD_REGISTER_NODE(hesiod::Slope, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::Stamping, "Primitive/Authoring");
  HSD_REGISTER_NODE(hesiod::Step, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::Stratify, "Erosion/Stratify");
  HSD_REGISTER_NODE(hesiod::StratifyOblique, "Erosion/Stratify");
  HSD_REGISTER_NODE(hesiod::Thermal, "Erosion/Thermal");
  HSD_REGISTER_NODE(hesiod::ThermalRib, "Erosion/Thermal");
  HSD_REGISTER_NODE(hesiod::Translate, "Operator/Transform");
  HSD_REGISTER_NODE(hesiod::Unpack, "Routing");
  HSD_REGISTER_NODE(hesiod::Unsphericity, "Features/Landform");
  HSD_REGISTER_NODE(hesiod::ValleyWidth, "Features/Landform");
  HSD_REGISTER_NODE(hesiod::Warp, "Operator/Transform");
  HSD_REGISTER_NODE(hesiod::WarpDownslope, "Operator/Transform");
  HSD_REGISTER_NODE(hesiod::WaveDune, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::WaveSine, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::WaveSquare, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::WaveTriangular, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::White, "Primitive/Random");
  HSD_REGISTER_NODE(hesiod::WhiteDensityMap, "Primitive/Random");
  HSD_REGISTER_NODE(hesiod::WhiteSparse, "Primitive/Random");
  HSD_REGISTER_NODE(hesiod::ZeroedEdges, "Math/Boundaries");
  HSD_REGISTER_NODE(hesiod::Zoom, "Operator/Transform");

  log->debug("node registry initialized");

  return ret;
}

} // namespace hesiod
