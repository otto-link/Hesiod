/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/utils.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

std::shared_ptr<gnode::Node> create_view_node_from_type(std::string node_type,
                                                        std::string node_id,
                                                        hmap::Vec2<int> shape,
                                                        hmap::Vec2<int> tiling,
                                                        float           overlap)
{

  // clang-format off
  switch(str2int(node_type.c_str()))
  {
  case str2int("Abs"):
    return std::make_shared<hesiod::vnode::ViewAbs>(node_id);
  case str2int("AbsSmooth"):
    return std::make_shared<hesiod::vnode::ViewAbsSmooth>(node_id);
  case str2int("BaseElevation"):
    return std::make_shared<hesiod::vnode::ViewBaseElevation>(node_id, shape, tiling, overlap);
  case str2int("BezierPath"):
    return std::make_shared<hesiod::vnode::ViewBezierPath>(node_id);
  case str2int("BiquadPulse"):
    return std::make_shared<hesiod::vnode::ViewBiquadPulse>(node_id, shape, tiling, overlap);
  case str2int("Blend"):
    return std::make_shared<hesiod::vnode::ViewBlend>(node_id);
  case str2int("Brush"):
    return std::make_shared<hesiod::vnode::ViewBrush>(node_id, shape, tiling, overlap);
  case str2int("Bump"):
    return std::make_shared<hesiod::vnode::ViewBump>(node_id, shape, tiling, overlap);
  case str2int("Caldera"):
    return std::make_shared<hesiod::vnode::ViewCaldera>(node_id, shape, tiling, overlap);
  case str2int("Checkerboard"):
    return std::make_shared<hesiod::vnode::ViewCheckerboard>(node_id, shape, tiling, overlap);
  case str2int("Clamp"):
    return std::make_shared<hesiod::vnode::ViewClamp>(node_id);
  case str2int("Clone"):
    return std::make_shared<hesiod::vnode::ViewClone>(node_id);
  case str2int("Cloud"):
    return std::make_shared<hesiod::vnode::ViewCloud>(node_id);
  case str2int("CloudToArrayInterp"):
    return std::make_shared<hesiod::vnode::ViewCloudToArrayInterp>(node_id, shape, tiling, overlap);
  case str2int("Colorize"):
    return std::make_shared<hesiod::vnode::ViewColorize>(node_id);
  case str2int("ColorizeSolid"):
    return std::make_shared<hesiod::vnode::ViewColorizeSolid>(node_id);
  case str2int("CombineMask"):
    return std::make_shared<hesiod::vnode::ViewCombineMask>(node_id);
  case str2int("ConvolveSVD"):
    return std::make_shared<hesiod::vnode::ViewConvolveSVD>(node_id);
  case str2int("Debug"):
    return std::make_shared<hesiod::vnode::ViewDebug>(node_id);
  case str2int("Dendry"):
    return std::make_shared<hesiod::vnode::ViewDendry>(node_id, shape, tiling, overlap);
  case str2int("DepressionFilling"):
    return std::make_shared<hesiod::vnode::ViewDepressionFilling>(node_id);
  case str2int("DigPath"):
    return std::make_shared<hesiod::vnode::ViewDigPath>(node_id);
  case str2int("DistanceTransform"):
    return std::make_shared<hesiod::vnode::ViewDistanceTransform>(node_id);
  case str2int("Equalize"):
    return std::make_shared<hesiod::vnode::ViewEqualize>(node_id);
  case str2int("ErosionMaps"):
    return std::make_shared<hesiod::vnode::ViewErosionMaps>(node_id);
  case str2int("ExpandShrinkDirectional"):
    return std::make_shared<hesiod::vnode::ViewExpandShrinkDirectional>(node_id);
  case str2int("ExpandShrink"):
    return std::make_shared<hesiod::vnode::ViewExpandShrink>(node_id);
  case str2int("Export"):
    return std::make_shared<hesiod::vnode::ViewExport>(node_id);
  case str2int("ExportOBJ"):
    return std::make_shared<hesiod::vnode::ViewExportOBJ>(node_id);
  case str2int("ExportRGB"):
    return std::make_shared<hesiod::vnode::ViewExportRGB>(node_id);
  case str2int("Faceted"):
    return std::make_shared<hesiod::vnode::ViewFaceted>(node_id);
  case str2int("FractalizePath"):
    return std::make_shared<hesiod::vnode::ViewFractalizePath>(node_id);
  case str2int("GaborNoise"):
    return std::make_shared<hesiod::vnode::ViewGaborNoise>(node_id, shape, tiling, overlap);
  case str2int("Gain"):
    return std::make_shared<hesiod::vnode::ViewGain>(node_id);
  case str2int("GammaCorrection"):
    return std::make_shared<hesiod::vnode::ViewGammaCorrection>(node_id);
  case str2int("GammaCorrectionLocal"):
    return std::make_shared<hesiod::vnode::ViewGammaCorrectionLocal>(node_id);
  case str2int("GaussianDecay"):
    return std::make_shared<hesiod::vnode::ViewGaussianDecay>(node_id);
  case str2int("GaussianPulse"):
    return std::make_shared<hesiod::vnode::ViewGaussianPulse>(node_id, shape, tiling, overlap);
  case str2int("Geomorphons"):
    return std::make_shared<hesiod::vnode::ViewGeomorphons>(node_id);
  case str2int("Gradient"):
    return std::make_shared<hesiod::vnode::ViewGradient>(node_id);
  case str2int("GradientAngle"):
    return std::make_shared<hesiod::vnode::ViewGradientAngle>(node_id);
  case str2int("GradientNorm"):
    return std::make_shared<hesiod::vnode::ViewGradientNorm>(node_id);
  case str2int("GradientTalus"):
    return std::make_shared<hesiod::vnode::ViewGradientTalus>(node_id);
  case str2int("HydraulicAlgebric"):
    return std::make_shared<hesiod::vnode::ViewHydraulicAlgebric>(node_id);
  case str2int("HydraulicParticle"):
    return std::make_shared<hesiod::vnode::ViewHydraulicParticle>(node_id);
  case str2int("HydraulicRidge"):
    return std::make_shared<hesiod::vnode::ViewHydraulicRidge>(node_id);
  case str2int("HydraulicStream"):
    return std::make_shared<hesiod::vnode::ViewHydraulicStream>(node_id);
  case str2int("HydraulicStreamLog"):
    return std::make_shared<hesiod::vnode::ViewHydraulicStreamLog>(node_id);
  case str2int("HydraulicVpipes"):
    return std::make_shared<hesiod::vnode::ViewHydraulicVpipes>(node_id);
  case str2int("Import"):
    return std::make_shared<hesiod::vnode::ViewImport>(node_id, shape, tiling, overlap);
  case str2int("Inverse"):
    return std::make_shared<hesiod::vnode::ViewInverse>(node_id);
  case str2int("Kernel"):
    return std::make_shared<hesiod::vnode::ViewKernel>(node_id);
  case str2int("KmeansClustering2"):
    return std::make_shared<hesiod::vnode::ViewKmeansClustering2>(node_id);
  case str2int("KmeansClustering3"):
    return std::make_shared<hesiod::vnode::ViewKmeansClustering3>(node_id);
  case str2int("Laplace"):
    return std::make_shared<hesiod::vnode::ViewLaplace>(node_id);
  case str2int("LaplaceEdgePreserving"):
    return std::make_shared<hesiod::vnode::ViewLaplaceEdgePreserving>(node_id);
  case str2int("Lerp"):
    return std::make_shared<hesiod::vnode::ViewLerp>(node_id);
  case str2int("MakeBinary"):
    return std::make_shared<hesiod::vnode::ViewMakeBinary>(node_id);
  case str2int("MeanderizePath"):
    return std::make_shared<hesiod::vnode::ViewMeanderizePath>(node_id);
  case str2int("MeanLocal"):
    return std::make_shared<hesiod::vnode::ViewMeanLocal>(node_id);
  case str2int("Median3x3"):
    return std::make_shared<hesiod::vnode::ViewMedian3x3>(node_id);
  case str2int("MinimumLocal"):
    return std::make_shared<hesiod::vnode::ViewMinimumLocal>(node_id);
  case str2int("MixRGB"):
    return std::make_shared<hesiod::vnode::ViewMixRGB>(node_id);
  case str2int("Noise"):
    return std::make_shared<hesiod::vnode::ViewNoise>(node_id, shape, tiling, overlap);
  case str2int("NoiseFbm"):
    return std::make_shared<hesiod::vnode::ViewNoiseFbm>(node_id, shape, tiling, overlap);
  case str2int("NoiseIq"):
    return std::make_shared<hesiod::vnode::ViewNoiseIq>(node_id, shape, tiling, overlap);
  case str2int("NoiseJordan"):
    return std::make_shared<hesiod::vnode::ViewNoiseJordan>(node_id, shape, tiling, overlap);
  case str2int("NoisePingpong"):
    return std::make_shared<hesiod::vnode::ViewNoisePingpong>(node_id, shape, tiling, overlap);
  case str2int("NoiseRidged"):
    return std::make_shared<hesiod::vnode::ViewNoiseRidged>(node_id, shape, tiling, overlap);
  case str2int("NoiseSwiss"):
    return std::make_shared<hesiod::vnode::ViewNoiseSwiss>(node_id, shape, tiling, overlap);
  case str2int("NormalDisplacement"):
    return std::make_shared<hesiod::vnode::ViewNormalDisplacement>(node_id);
  case str2int("OneMinus"):
    return std::make_shared<hesiod::vnode::ViewOneMinus>(node_id);
  case str2int("Path"):
    return std::make_shared<hesiod::vnode::ViewPath>(node_id);
  case str2int("PathFinding"):
    return std::make_shared<hesiod::vnode::ViewPathFinding>(node_id);
  case str2int("PathToHeightmap"):
    return std::make_shared<hesiod::vnode::ViewPathToHeightmap>(node_id, shape, tiling, overlap);
  case str2int("Peak"):
    return std::make_shared<hesiod::vnode::ViewPeak>(node_id, shape, tiling, overlap);
  case str2int("Plateau"):
    return std::make_shared<hesiod::vnode::ViewPlateau>(node_id);
  case str2int("Preview"):
    return std::make_shared<hesiod::vnode::ViewPreview>(node_id);
  case str2int("PreviewColorize"):
    return std::make_shared<hesiod::vnode::ViewPreviewColorize>(node_id);
  case str2int("RecastCanyon"):
    return std::make_shared<hesiod::vnode::ViewRecastCanyon>(node_id);
  case str2int("RecastCliff"):
    return std::make_shared<hesiod::vnode::ViewRecastCliff>(node_id);
  case str2int("RecastCliffDirectional"):
    return std::make_shared<hesiod::vnode::ViewRecastCliffDirectional>(node_id);
  case str2int("RecastPeak"):
    return std::make_shared<hesiod::vnode::ViewRecastPeak>(node_id);
  case str2int("RecastRockySlopes"):
    return std::make_shared<hesiod::vnode::ViewRecastRockySlopes>(node_id);
  case str2int("Recurve"):
    return std::make_shared<hesiod::vnode::ViewRecurve>(node_id);
  case str2int("RecurveKura"):
    return std::make_shared<hesiod::vnode::ViewRecurveKura>(node_id);
  case str2int("RecurveS"):
    return std::make_shared<hesiod::vnode::ViewRecurveS>(node_id);
  case str2int("RelativeElevation"):
    return std::make_shared<hesiod::vnode::ViewRelativeElevation>(node_id);
  case str2int("Remap"):
    return std::make_shared<hesiod::vnode::ViewRemap>(node_id);
  case str2int("Rescale"):
    return std::make_shared<hesiod::vnode::ViewRescale>(node_id);
  case str2int("Rugosity"):
    return std::make_shared<hesiod::vnode::ViewRugosity>(node_id);
  case str2int("SedimentDeposition"):
    return std::make_shared<hesiod::vnode::ViewSedimentDeposition>(node_id);
  case str2int("SelectBlobLog"):
    return std::make_shared<hesiod::vnode::ViewSelectBlobLog>(node_id);
  case str2int("SelectCavities"):
    return std::make_shared<hesiod::vnode::ViewSelectCavities>(node_id);
  case str2int("SelectElevationSlope"):
    return std::make_shared<hesiod::vnode::ViewSelectElevationSlope>(node_id);
  case str2int("SelectGradientNorm"):
    return std::make_shared<hesiod::vnode::ViewSelectGradientNorm>(node_id);
  case str2int("SelectEq"):
    return std::make_shared<hesiod::vnode::ViewSelectEq>(node_id);
  case str2int("SelectInterval"):
    return std::make_shared<hesiod::vnode::ViewSelectInterval>(node_id);
  case str2int("SelectPulse"):
    return std::make_shared<hesiod::vnode::ViewSelectPulse>(node_id);
  case str2int("SelectRivers"):
    return std::make_shared<hesiod::vnode::ViewSelectRivers>(node_id);
  case str2int("SelectTransitions"):
    return std::make_shared<hesiod::vnode::ViewSelectTransitions>(node_id);
  case str2int("SharpenCone"):
    return std::make_shared<hesiod::vnode::ViewSharpenCone>(node_id);
  case str2int("Slope"):
    return std::make_shared<hesiod::vnode::ViewSlope>(node_id, shape, tiling, overlap);
  case str2int("SmoothCpulse"):
    return std::make_shared<hesiod::vnode::ViewSmoothCpulse>(node_id);
  case str2int("SmoothFill"):
    return std::make_shared<hesiod::vnode::ViewSmoothFill>(node_id);
  case str2int("SmoothFillHoles"):
    return std::make_shared<hesiod::vnode::ViewSmoothFillHoles>(node_id);
  case str2int("SmoothFillSmearPeaks"):
    return std::make_shared<hesiod::vnode::ViewSmoothFillSmearPeaks>(node_id);
  case str2int("SteepenConvective"):
    return std::make_shared<hesiod::vnode::ViewSteepenConvective>(node_id);
  case str2int("Step"):
    return std::make_shared<hesiod::vnode::ViewStep>(node_id, shape, tiling, overlap);
  case str2int("StratifyMultiscale"):
    return std::make_shared<hesiod::vnode::ViewStratifyMultiscale>(node_id);
  case str2int("StratifyOblique"):
    return std::make_shared<hesiod::vnode::ViewStratifyOblique>(node_id);
  case str2int("Thermal"):
    return std::make_shared<hesiod::vnode::ViewThermal>(node_id);
  case str2int("ThermalAutoBedrock"):
    return std::make_shared<hesiod::vnode::ViewThermalAutoBedrock>(node_id);
  case str2int("ThermalFlatten"):
    return std::make_shared<hesiod::vnode::ViewThermalFlatten>(node_id);
  case str2int("ThermalScree"):
    return std::make_shared<hesiod::vnode::ViewThermalScree>(node_id);
  case str2int("ToKernel"):
    return std::make_shared<hesiod::vnode::ViewToKernel>(node_id);
  case str2int("ToMask"):
    return std::make_shared<hesiod::vnode::ViewToMask>(node_id);
  case str2int("ValleyWidth"):
    return std::make_shared<hesiod::vnode::ViewValleyWidth>(node_id);
  case str2int("WaveDune"):
    return std::make_shared<hesiod::vnode::ViewWaveDune>(node_id, shape, tiling, overlap);
  case str2int("WaveSine"):
    return std::make_shared<hesiod::vnode::ViewWaveSine>(node_id, shape, tiling, overlap);
  case str2int("WaveSquare"):
    return std::make_shared<hesiod::vnode::ViewWaveSquare>(node_id, shape, tiling, overlap);
  case str2int("WaveTriangular"):
    return std::make_shared<hesiod::vnode::ViewWaveTriangular>(node_id, shape, tiling, overlap);
  case str2int("Warp"):
    return std::make_shared<hesiod::vnode::ViewWarp>(node_id);
  case str2int("WarpDownslope"):
    return std::make_shared<hesiod::vnode::ViewWarpDownslope>(node_id);
  case str2int("White"):
    return std::make_shared<hesiod::vnode::ViewWhite>(node_id, shape, tiling, overlap);
  case str2int("WhiteDensityMap"):
    return std::make_shared<hesiod::vnode::ViewWhiteDensityMap>(node_id);
  case str2int("WhiteSparse"):
    return std::make_shared<hesiod::vnode::ViewWhiteSparse>(node_id, shape, tiling, overlap);
  case str2int("Wrinkle"):
    return std::make_shared<hesiod::vnode::ViewWrinkle>(node_id);
  case str2int("ZeroedEdges"):
    return std::make_shared<hesiod::vnode::ViewZeroedEdges>(node_id);
  default:
    // clang-format on
    {
      LOG_ERROR("unknown node type: [%s]", node_type.c_str());
      throw std::runtime_error("adding an unknown node type");
      break;
    }
  }
}

} // namespace hesiod::vnode
