/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "gnode.hpp"
#include "macrologger.h"

#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

namespace hesiod::vnode
{

std::string ViewTree::add_view_node(std::string control_node_type,
                                    std::string node_id)
{
  std::string id;
  if (node_id == "")
    id = control_node_type + "##" + this->get_new_id();
  else
    id = node_id;

  LOG_DEBUG("adding node type: %s", control_node_type.c_str());

  // clang-format off
  if (control_node_type == "AlterElevation")
    this->add_node(std::make_shared<hesiod::vnode::ViewAlterElevation>(id));
  else if (control_node_type == "BaseElevation")
    this->add_node(std::make_shared<hesiod::vnode::ViewBaseElevation>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "BezierPath")
    this->add_node(std::make_shared<hesiod::vnode::ViewBezierPath>(id));
  else if (control_node_type == "BiquadPulse")
    this->add_node(std::make_shared<hesiod::vnode::ViewBiquadPulse>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "Blend")
    this->add_node(std::make_shared<hesiod::vnode::ViewBlend>(id));
  else if (control_node_type == "Brush")
    this->add_node(std::make_shared<hesiod::vnode::ViewBrush>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "Bump")
    this->add_node(std::make_shared<hesiod::vnode::ViewBump>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "Caldera")
    this->add_node(std::make_shared<hesiod::vnode::ViewCaldera>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "Checkerboard")
    this->add_node(std::make_shared<hesiod::vnode::ViewCheckerboard>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "Clamp")
    this->add_node(std::make_shared<hesiod::vnode::ViewClamp>(id));
  else if (control_node_type == "Clone")
    this->add_node(std::make_shared<hesiod::vnode::ViewClone>(id));
  else if (control_node_type == "Cloud")
    this->add_node(std::make_shared<hesiod::vnode::ViewCloud>(id));
  else if (control_node_type == "CloudToArrayInterp")
    this->add_node(std::make_shared<hesiod::vnode::ViewCloudToArrayInterp>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "Colorize")
    this->add_node(std::make_shared<hesiod::vnode::ViewColorize>(id));
  else if (control_node_type == "CombineMask")
    this->add_node(std::make_shared<hesiod::vnode::ViewCombineMask>(id));
  else if (control_node_type == "ConvolveSVD")
    this->add_node(std::make_shared<hesiod::vnode::ViewConvolveSVD>(id));
  else if (control_node_type == "Debug")
    this->add_node(std::make_shared<hesiod::vnode::ViewDebug>(id));
  else if (control_node_type == "DepressionFilling")
    this->add_node(std::make_shared<hesiod::vnode::ViewDepressionFilling>(id));
  else if (control_node_type == "DigPath")
    this->add_node(std::make_shared<hesiod::vnode::ViewDigPath>(id));
  else if (control_node_type == "DistanceTransform")
    this->add_node(std::make_shared<hesiod::vnode::ViewDistanceTransform>(id));
  else if (control_node_type == "Equalize")
    this->add_node(std::make_shared<hesiod::vnode::ViewEqualize>(id));
  else if (control_node_type == "ErosionMaps")
    this->add_node(std::make_shared<hesiod::vnode::ViewErosionMaps>(id));
  else if (control_node_type == "ExpandShrinkDirectional")
    this->add_node(std::make_shared<hesiod::vnode::ViewExpandShrinkDirectional>(id));
  else if (control_node_type == "ExpandShrink")
    this->add_node(std::make_shared<hesiod::vnode::ViewExpandShrink>(id));
  else if (control_node_type == "Export")
    this->add_node(std::make_shared<hesiod::vnode::ViewExport>(id));
  else if (control_node_type == "ExportRGB")
    this->add_node(std::make_shared<hesiod::vnode::ViewExportRGB>(id));
  else if (control_node_type == "FbmPerlin")
    this->add_node(std::make_shared<hesiod::vnode::ViewFbmPerlin>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "FbmSimplex")
    this->add_node(std::make_shared<hesiod::vnode::ViewFbmSimplex>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "FbmWorley")
    this->add_node(std::make_shared<hesiod::vnode::ViewFbmWorley>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "FractalizePath")
    this->add_node(std::make_shared<hesiod::vnode::ViewFractalizePath>(id));
  else if (control_node_type == "GaborNoise")
    this->add_node(std::make_shared<hesiod::vnode::ViewGaborNoise>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "Gain")
    this->add_node(std::make_shared<hesiod::vnode::ViewGain>(id));
  else if (control_node_type == "GammaCorrection")
    this->add_node(std::make_shared<hesiod::vnode::ViewGammaCorrection>(id));
  else if (control_node_type == "GammaCorrectionLocal")
    this->add_node(std::make_shared<hesiod::vnode::ViewGammaCorrectionLocal>(id));
  else if (control_node_type == "GaussianPulse")
    this->add_node(std::make_shared<hesiod::vnode::ViewGaussianPulse>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "Gradient")
    this->add_node(std::make_shared<hesiod::vnode::ViewGradient>(id));
  else if (control_node_type == "GradientAngle")
    this->add_node(std::make_shared<hesiod::vnode::ViewGradientAngle>(id));
  else if (control_node_type == "GradientNorm")
    this->add_node(std::make_shared<hesiod::vnode::ViewGradientNorm>(id));
  else if (control_node_type == "GradientTalus")
    this->add_node(std::make_shared<hesiod::vnode::ViewGradientTalus>(id));
  else if (control_node_type == "HydraulicAlgebric")
    this->add_node(std::make_shared<hesiod::vnode::ViewHydraulicAlgebric>(id));
  else if (control_node_type == "HydraulicParticle")
    this->add_node(std::make_shared<hesiod::vnode::ViewHydraulicParticle>(id));
  else if (control_node_type == "HydraulicRidge")
    this->add_node(std::make_shared<hesiod::vnode::ViewHydraulicRidge>(id));
  else if (control_node_type == "HydraulicStream")
    this->add_node(std::make_shared<hesiod::vnode::ViewHydraulicStream>(id));
  else if (control_node_type == "HydraulicStreamLog")
    this->add_node(std::make_shared<hesiod::vnode::ViewHydraulicStreamLog>(id));
  else if (control_node_type == "HydraulicVpipes")
    this->add_node(std::make_shared<hesiod::vnode::ViewHydraulicVpipes>(id));
  else if (control_node_type == "Import")
    this->add_node(std::make_shared<hesiod::vnode::ViewImport>(id, shape, tiling, overlap));
  else if (control_node_type == "Kernel")
    this->add_node(std::make_shared<hesiod::vnode::ViewKernel>(id));
  else if (control_node_type == "KmeansClustering2")
    this->add_node(std::make_shared<hesiod::vnode::ViewKmeansClustering2>(id));
  else if (control_node_type == "KmeansClustering3")
    this->add_node(std::make_shared<hesiod::vnode::ViewKmeansClustering3>(id));
  else if (control_node_type == "Laplace")
    this->add_node(std::make_shared<hesiod::vnode::ViewLaplace>(id));
  else if (control_node_type == "LaplaceEdgePreserving")
    this->add_node(std::make_shared<hesiod::vnode::ViewLaplaceEdgePreserving>(id));
  else if (control_node_type == "Lerp")
    this->add_node(std::make_shared<hesiod::vnode::ViewLerp>(id));
  else if (control_node_type == "MakeBinary")
    this->add_node(std::make_shared<hesiod::vnode::ViewMakeBinary>(id));
  else if (control_node_type == "MeanderizePath")
    this->add_node(std::make_shared<hesiod::vnode::ViewMeanderizePath>(id));
  else if (control_node_type == "MeanLocal")
    this->add_node(std::make_shared<hesiod::vnode::ViewMeanLocal>(id));
  else if (control_node_type == "Median3x3")
    this->add_node(std::make_shared<hesiod::vnode::ViewMedian3x3>(id));
  else if (control_node_type == "MinimumLocal")
    this->add_node(std::make_shared<hesiod::vnode::ViewMinimumLocal>(id));
  else if (control_node_type == "MixRGB")
    this->add_node(std::make_shared<hesiod::vnode::ViewMixRGB>(id));
  else if (control_node_type == "NormalDisplacement")
    this->add_node(std::make_shared<hesiod::vnode::ViewNormalDisplacement>(id));
  else if (control_node_type == "OneMinus")
    this->add_node(std::make_shared<hesiod::vnode::ViewOneMinus>(id));
  else if (control_node_type == "Path")
    this->add_node(std::make_shared<hesiod::vnode::ViewPath>(id));
  else if (control_node_type == "PathFinding")
    this->add_node(std::make_shared<hesiod::vnode::ViewPathFinding>(id));
  else if (control_node_type == "PathToHeightmap")
    this->add_node(std::make_shared<hesiod::vnode::ViewPathToHeightmap>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "Perlin")
    this->add_node(std::make_shared<hesiod::vnode::ViewPerlin>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "PerlinBillow")
    this->add_node(std::make_shared<hesiod::vnode::ViewPerlinBillow>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "Plateau")
    this->add_node(std::make_shared<hesiod::vnode::ViewPlateau>(id));
  else if (control_node_type == "Preview")
    this->add_node(std::make_shared<hesiod::vnode::ViewPreview>(id));
  else if (control_node_type == "PreviewColorize")
    this->add_node(std::make_shared<hesiod::vnode::ViewPreviewColorize>(id));
  else if (control_node_type == "RecastCanyon")
    this->add_node(std::make_shared<hesiod::vnode::ViewRecastCanyon>(id));
  else if (control_node_type == "Recurve")
    this->add_node(std::make_shared<hesiod::vnode::ViewRecurve>(id));
  else if (control_node_type == "RecurveKura")
    this->add_node(std::make_shared<hesiod::vnode::ViewRecurveKura>(id));
  else if (control_node_type == "RecurveS")
    this->add_node(std::make_shared<hesiod::vnode::ViewRecurveS>(id));
  else if (control_node_type == "RelativeElevation")
    this->add_node(std::make_shared<hesiod::vnode::ViewRelativeElevation>(id));
  else if (control_node_type == "Remap")
    this->add_node(std::make_shared<hesiod::vnode::ViewRemap>(id));
  else if (control_node_type == "RidgedPerlin")
    this->add_node(std::make_shared<hesiod::vnode::ViewRidgedPerlin>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "Rugosity")
    this->add_node(std::make_shared<hesiod::vnode::ViewRugosity>(id));
  else if (control_node_type == "SedimentDeposition")
    this->add_node(std::make_shared<hesiod::vnode::ViewSedimentDeposition>(id));
  else if (control_node_type == "SelectCavities")
    this->add_node(std::make_shared<hesiod::vnode::ViewSelectCavities>(id));
  else if (control_node_type == "SelectGradientNorm")
    this->add_node(std::make_shared<hesiod::vnode::ViewSelectGradientNorm>(id));
  else if (control_node_type == "SelectEq")
    this->add_node(std::make_shared<hesiod::vnode::ViewSelectEq>(id));
  else if (control_node_type == "SelectInterval")
    this->add_node(std::make_shared<hesiod::vnode::ViewSelectInterval>(id));
  else if (control_node_type == "SelectRivers")
    this->add_node(std::make_shared<hesiod::vnode::ViewSelectRivers>(id));
  else if (control_node_type == "SelectTransitions")
    this->add_node(std::make_shared<hesiod::vnode::ViewSelectTransitions>(id));
  else if (control_node_type == "Simplex")
    this->add_node(std::make_shared<hesiod::vnode::ViewSimplex>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "Slope")
    this->add_node(std::make_shared<hesiod::vnode::ViewSlope>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "SmoothCpulse")
    this->add_node(std::make_shared<hesiod::vnode::ViewSmoothCpulse>(id));
  else if (control_node_type == "SmoothFill")
    this->add_node(std::make_shared<hesiod::vnode::ViewSmoothFill>(id));
  else if (control_node_type == "SmoothFillHoles")
    this->add_node(std::make_shared<hesiod::vnode::ViewSmoothFillHoles>(id));
  else if (control_node_type == "SmoothFillSmearPeaks")
    this->add_node(std::make_shared<hesiod::vnode::ViewSmoothFillSmearPeaks>(id));
  else if (control_node_type == "SteepenConvective")
    this->add_node(std::make_shared<hesiod::vnode::ViewSteepenConvective>(id));
  else if (control_node_type == "Step")
    this->add_node(std::make_shared<hesiod::vnode::ViewStep>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "StratifyMultiscale")
    this->add_node(std::make_shared<hesiod::vnode::ViewStratifyMultiscale>(id));
  else if (control_node_type == "StratifyOblique")
    this->add_node(std::make_shared<hesiod::vnode::ViewStratifyOblique>(id));
  else if (control_node_type == "Thermal")
    this->add_node(std::make_shared<hesiod::vnode::ViewThermal>(id));
  else if (control_node_type == "ThermalAutoBedrock")
    this->add_node(std::make_shared<hesiod::vnode::ViewThermalAutoBedrock>(id));
  else if (control_node_type == "ThermalScree")
    this->add_node(std::make_shared<hesiod::vnode::ViewThermalScree>(id));
  else if (control_node_type == "ToMask")
    this->add_node(std::make_shared<hesiod::vnode::ViewToMask>(id));
  else if (control_node_type == "ValleyWidth")
    this->add_node(std::make_shared<hesiod::vnode::ViewValleyWidth>(id));
  else if (control_node_type == "ValueNoiseDelaunay")
    this->add_node(std::make_shared<hesiod::vnode::ViewValueNoiseDelaunay>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "ValueNoiseLinear")
    this->add_node(std::make_shared<hesiod::vnode::ViewValueNoiseLinear>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "ValueNoiseThinplate")
    this->add_node(std::make_shared<hesiod::vnode::ViewValueNoiseThinplate>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "WaveDune")
    this->add_node(std::make_shared<hesiod::vnode::ViewWaveDune>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "WaveSine")
    this->add_node(std::make_shared<hesiod::vnode::ViewWaveSine>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "Warp")
    this->add_node(std::make_shared<hesiod::vnode::ViewWarp>(id));
  else if (control_node_type == "WarpDownslope")
    this->add_node(std::make_shared<hesiod::vnode::ViewWarpDownslope>(id));
  else if (control_node_type == "White")
    this->add_node(std::make_shared<hesiod::vnode::ViewWhite>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "WhiteDensityMap")
    this->add_node(std::make_shared<hesiod::vnode::ViewWhiteDensityMap>(id));
  else if (control_node_type == "WhiteSparse")
    this->add_node(std::make_shared<hesiod::vnode::ViewWhiteSparse>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "Worley")
    this->add_node(std::make_shared<hesiod::vnode::ViewWorley>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "WorleyDouble")
    this->add_node(std::make_shared<hesiod::vnode::ViewWorleyDouble>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "WorleyValue")
    this->add_node(std::make_shared<hesiod::vnode::ViewWorleyValue>(id, this->shape, this->tiling, this->overlap));
  else if (control_node_type == "ZeroedEdges")
    this->add_node(std::make_shared<hesiod::vnode::ViewZeroedEdges>(id));
  else
  // clang-format on
  {
    LOG_ERROR("unknown node type: [%s]", control_node_type.c_str());
    throw std::runtime_error("adding an unknown node type");
  }

  return id;
}

} // namespace hesiod::vnode
