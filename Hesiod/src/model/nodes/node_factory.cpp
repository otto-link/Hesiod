/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>
#include <stdexcept>

#include "attributes/widgets/attributes_widget.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/utils.hpp"

#include "hesiod/gui/gui_utils.hpp"

// specific nodes
#include "hesiod/model/nodes/broadcast_node.hpp"
#include "hesiod/model/nodes/receive_node.hpp"

#define SETUP_NODE(NodeType, node_type)                                                  \
  case str2int(#NodeType):                                                               \
    setup_##node_type##_node(sptr.get());                                                \
    sptr->set_compute_fct(&compute_##node_type##_node);                                  \
    break;

namespace hesiod
{

// HELPERS
constexpr unsigned int str2int(const char *str, int h = 0)
{
  // https://stackoverflow.com/questions/16388510
  return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

void dump_node_inventory(const std::string &fname)
{
  LOG->trace("dump_node_inventory");

  std::map<std::string, std::string> ni = hesiod::get_node_inventory();

  // --- export to mermaid also

  std::fstream f;
  f.open(fname + ".csv", std::ios::out);

  for (auto [name, category] : ni)
  {
    f << name << ";";
    for (auto v : split_string(category, '/'))
      f << v << ";";
    f << "\n";
  }

  f.close();

  // --- export to mermaid also

  f.open(fname + ".mmd", std::ios::out);

  f << "---\n";
  f << "title: Hesiod nodes\n";
  f << "---\n";
  f << "flowchart LR\n";

  std::vector<std::string> edges = {};

  for (auto [name, category] : ni)
  {
    std::vector<std::string> categories = split_string(category, '/');
    categories.insert(categories.begin(), "NODES[(NODES)]");
    categories.push_back("#" + name + "([" + name + "])");

    for (size_t k = 0; k < categories.size() - 1; k++)
    {
      std::string new_edge = categories[k] + " --> " + categories[k + 1];

      if (std::find(edges.begin(), edges.end(), new_edge) == edges.end())
      {
        edges.push_back(new_edge);
        f << new_edge << "\n";
      }
    }

    f << "style " << "#" + name << " fill:#f9f\n";
  }

  f.close();
}

void dump_node_documentation_stub(const std::string           &fname,
                                  std::shared_ptr<ModelConfig> config)
{
  LOG->trace("dump_node_documentation_stub");

  std::map<std::string, std::string> ni = hesiod::get_node_inventory();

  nlohmann::json json;
  for (auto [name, category] : ni)
  {
    std::shared_ptr<gnode::Node> p_node = node_factory(name, config);
    hesiod::BaseNode *p_base_node = dynamic_cast<hesiod::BaseNode *>(p_node.get());
    json[name] = p_base_node->node_parameters_to_json();
  }

  std::fstream f;
  f.open(fname, std::ios::out);
  f << json.dump(4) << "\n";
  f.close();
}

void dump_node_settings_screenshots()
{
  LOG->trace("dump_node_settings_screenshots");

  // use default, not important
  auto config = std::make_shared<hesiod::ModelConfig>();

  std::map<std::string, std::string> ni = hesiod::get_node_inventory();

  nlohmann::json json;
  for (auto [name, category] : ni)
  {
    std::shared_ptr<gnode::Node> p_node = node_factory(name, config);
    hesiod::BaseNode *p_base_node = dynamic_cast<hesiod::BaseNode *>(p_node.get());

    attr::AttributesWidget *attributes_widget = new attr::AttributesWidget(
        p_base_node->get_attr_ref(),
        p_base_node->get_attr_ordered_key_ref());

    render_widget_screenshot(attributes_widget,
                             p_base_node->get_label() + "_settings.png",
                             QSize());
  }
}

std::map<std::string, std::string> get_node_inventory()
{
  std::map<std::string, std::string> node_inventory = {
      {"Abs", "Math/Base"},
      {"AbsSmooth", "Math/Base"},
      {"AccumulationCurvature", "Features/Landform"},
      {"BasaltField", "Primitive/Geological"},
      {"Blend", "Operator/Blend"},
      {"BlendPoissonBf", "Operator/Blend"},
      {"Border", "Operator/Morphology"},
      {"Brush", "Primitive/Authoring"},
      {"Broadcast", "Routing"},
      {"Bump", "Primitive/Function"},
      {"BumpLorentzian", "Primitive/Function"},
      {"Caldera", "Primitive/Geological"},
      {"Clamp", "Filter/Range"},
      {"Closing", "Operator/Morphology"},
      {"Cloud", "Geometry/Cloud"},
      {"CloudFromCsv", "Geometry/Cloud"},
      {"CloudMerge", "Geometry/Cloud"},
      {"CloudLattice", "Geometry/Cloud"},
      {"CloudRandom", "Geometry/Cloud"},
      {"CloudRandomDensity", "Geometry/Cloud"},
      {"CloudRandomDistance", "Geometry/Cloud"},
      {"CloudRandomPowerLaw", "Geometry/Cloud"},
      {"CloudRandomWeibull", "Geometry/Cloud"},
      {"CloudRemapValues", "Geometry/Cloud"},
      {"CloudSDF", "Geometry/Cloud"},
      {"CloudSetValuesFromBorderDistance", "Geometry/Cloud"},
      {"CloudSetValuesFromHeightmap", "Geometry/Cloud"},
      {"CloudSetValuesFromMinDistance", "Geometry/Cloud"},
      {"CloudShuffle", "Geometry/Cloud"},
      {"CloudToArrayInterp", "Geometry/Cloud"},
      {"CloudToPath", "Geometry/Cloud"},
      {"ColorizeCmap", "Texture"},
      {"ColorizeGradient", "Texture"},
      {"ColorizeSolid", "Texture"},
      {"CombineMask", "Mask"},
      {"Constant", "Primitive/Function"},
      {"ConvolveSVD", "Math/Convolution"},
      {"Cos", "Math/Base"},
      {"Crater", "Primitive/Geological"},
      {"Debug", "Debug"},
      {"Dendry", "Primitive/Coherent"},
      {"DepressionFilling", "Erosion"},
      {"Detrend", "WIP"},                     // Filter/Recurve
      {"DiffusionLimitedAggregation", "WIP"}, // Primitive/Coherent
      {"Dilation", "Operator/Morphology"},
      {"DirectionalBlur", "WIP"}, // Filter/Smoothing
      {"DistanceTransform", "Operator/Morphology"},
      {"Erosion", "Operator/Morphology"},
      {"ExpandShrink", "Filter/Recast"},
      {"ExportAsset", "IO/Files"},
      {"ExportAsCubemap", "WIP"}, // "IO/Files"},
      {"ExportCloud", "IO/Files"},
      {"ExportHeightmap", "IO/Files"},
      {"ExportNormalMap", "IO/Files"},
      {"ExportPath", "IO/Files"},
      {"ExportTexture", "IO/Files"},
      {"Falloff", "Math/Boundaries"},
      {"FillTalus", "WIP"}, // Operator/Transform
      {"FloodingFromBoundaries", "Hydrology"},
      {"FloodingFromPoint", "Hydrology"},
      {"FloodingUniformLevel", "Hydrology"},
      {"FlowStream", "WIP"}, // Hydrology
      {"Fold", "Filter/Recast"},
      {"GaborWaveFbm", "Primitive/Coherent"},
      {"Gain", "Filter/Recurve"},
      {"GammaCorrection", "Filter/Recurve"},
      {"GammaCorrectionLocal", "Filter/Recurve"},
      {"GaussianDecay", "Math/Base"},
      {"GaussianPulse", "Primitive/Function"},
      {"Gavoronoise", "Primitive/Coherent"},
      {"Gradient", "Math/Gradient"},
      {"GradientAngle", "Math/Gradient"},
      {"GradientNorm", "Math/Gradient"},
      {"GradientTalus", "Math/Gradient"},
      {"HeightmapToKernel", "Converter"},
      {"HeightmapToMask", "Converter"},
      {"HeightmapToNormalMap", "Converter"},
      {"HeightmapToRGBA", "Converter"},
      {"HydraulicBlur", "WIP"},     // "Erosion/Hydraulic"
      {"HydraulicMusgrave", "WIP"}, // "Erosion/Hydraulic"
      {"HydraulicParticle", "Erosion/Hydraulic"},
      {"HydraulicProcedural", "WIP"}, // "Erosion/Hydraulic"
      {"HydraulicSchott", "WIP"},     // "Erosion/Hydraulic"
      {"HydraulicStream", "WIP"},     // "Erosion/Hydraulic"
      {"HydraulicStreamLog", "Erosion/Hydraulic"},
      {"HydraulicStreamUpscaleAmplification", "WIP"}, // "Erosion/Hydraulic"
      {"HydraulicVpipes", "WIP"},                     // "Erosion/Hydraulic"
      {"ImportHeightmap", "IO/Files"},
      {"ImportTexture", "IO/Files"},
      {"Inverse", "Math/Base"},
      {"KernelDiskSmooth", "Primitive/Kernel"},
      {"KernelGabor", "Primitive/Kernel"},
      {"KernelPrim", "Primitive/Kernel"},
      {"KmeansClustering2", "Features/Clustering"},
      {"KmeansClustering3", "Features/Clustering"},
      {"Kuwahara", "WIP"}, // Filter/Smoothing
      {"Laplace", "Filter/Smoothing"},
      {"Lerp", "Math/Base"},
      {"MakeBinary", "Operator/Morphology"},
      {"MakePeriodic", "Operator/Tiling"},
      {"MakePeriodicStitching", "Operator/Tiling"},
      {"MeanShift", "WIP"},
      {"Median3x3", "Filter/Smoothing"},
      {"MedianPseudo", "WIP"},
      {"MixNormalMap", "Texture"},
      {"MixTexture", "Texture"},
      {"Mixer", "Operator/Blend"},
      {"MorphologicalGradient", "Operator/Morphology"},
      {"MorphologicalTopHat", "Operator/Morphology"},
      {"MountainRangeRadial", "Primitive/Geological"},
      {"Noise", "Primitive/Coherent"},
      {"NoiseFbm", "Primitive/Coherent"},
      {"NoiseIq", "Primitive/Coherent"},
      {"NoiseJordan", "Primitive/Coherent"},
      {"NoiseParberry", "Primitive/Coherent"},
      {"NoisePingpong", "Primitive/Coherent"},
      {"NoiseRidged", "Primitive/Coherent"},
      {"NoiseSwiss", "Primitive/Coherent"},
      {"NormalDisplacement", "Filter/Recast"},
      {"Opening", "Operator/Morphology"},
      {"Paraboloid", "Primitive/Function"},
      {"Path", "Geometry/Path"},
      {"PathBezier", "Geometry/Path"},
      {"PathBezierRound", "Geometry/Path"},
      {"PathBspline", "Geometry/Path"},
      {"PathDecasteljau", "Geometry/Path"},
      {"PathDig", "Geometry/Path"},
      {"PathFind", "Geometry/Path"},
      {"PathFractalize", "Geometry/Path"},
      {"PathFromCsv", "Geometry/Path"},
      {"PathMeanderize", "Geometry/Path"},
      {"PathResample", "Geometry/Path"},
      {"PathSDF", "Geometry/Path"},
      {"PathSmooth", "Geometry/Path"},
      {"PathToCloud", "Geometry/Path"},
      {"PathToHeightmap", "Geometry/Path"},
      {"Plateau", "Filter/Recurve"},
      {"PolygonField", "Primitive/Coherent"},
      {"PolygonFieldFbm", "Primitive/Coherent"},
      {"Preview", "Debug"},
      {"QuiltingBlend", "Operator/Resynthesis"},
      {"QuiltingExpand", "Operator/Resynthesis"},
      {"QuiltingShuffle", "Operator/Resynthesis"},
      {"RadialDisplacementToXy", "Math"},
      {"RecastCanyon", "Filter/Recast"},
      {"RecastCliff", "Filter/Recast"},
      {"RecastCliffDirectional", "Filter/Recast"},
      {"RecastCracks", "Filter/Recast"},
      {"RecastSag", "Filter/Recast"},
      {"Receive", "Routing"},
      {"Recurve", "Filter/Recurve"},
      {"RecurveKura", "Filter/Recurve"},
      {"RecurveS", "Filter/Recurve"},
      {"RelativeDistanceFromSkeleton", "Operator/Morphology"},
      {"RelativeElevation", "Features/Landform"},
      {"Remap", "Filter/Range"},
      {"Rescale", "Filter/Range"},
      {"Reverse", "Math/Base"},
      {"ReverseAboveThreshold", "Filter/Recurve"},
      {"ReverseMidpoint", "WIP"}, // Primitive/Authoring
      {"Ridgelines", "Primitive/Authoring"},
      {"Rift", "Primitive/Function"},
      {"Rifts", "Erosion/Hydraulic"},
      {"HydraulicProcedural", "WIP"}, // "Erosion/Hydraulic"
      {"HydraulicSchott", "WIP"},     // "Erosion/Hydraulic"
      {"HydraulicStream", "WIP"},     // "Erosion/Hydraulic"
      {"HydraulicStreamLog", "Erosion/Hydraulic"},
      {"HydraulicStreamUpscaleAmplification", "WIP"}, // "Erosion/Hydraulic"
      {"HydraulicVpipes", "WIP"},                     // "Erosion/Hydraulic"
      {"ImportHeightmap", "IO/Files"},
      {"ImportTexture", "IO/Files"},
      {"Inverse", "Math/Base"},
      {"KernelDiskSmooth", "Primitive/Kernel"},
      {"KernelGabor", "Primitive/Kernel"},
      {"KernelPrim", "Primitive/Kernel"},
      {"KmeansClustering2", "Features/Clustering"},
      {"KmeansClustering3", "Features/Clustering"},
      {"Kuwahara", "WIP"}, // Filter/Smoothing
      {"Laplace", "Filter/Smoothing"},
      {"Lerp", "Math/Base"},
      {"MakeBinary", "Operator/Morphology"},
      {"MakePeriodic", "Operator/Tiling"},
      {"MakePeriodicStitching", "Operator/Tiling"},
      {"MeanShift", "WIP"},
      {"Median3x3", "Filter/Smoothing"},
      {"MedianPseudo", "WIP"},
      {"MixNormalMap", "Texture"},
      {"MixTexture", "Texture"},
      {"Mixer", "Operator/Blend"},
      {"MorphologicalGradient", "Operator/Morphology"},
      {"MorphologicalTopHat", "Operator/Morphology"},
      {"MountainRangeRadial", "Primitive/Geological"},
      {"Noise", "Primitive/Coherent"},
      {"NoiseFbm", "Primitive/Coherent"},
      {"NoiseIq", "Primitive/Coherent"},
      {"NoiseJordan", "Primitive/Coherent"},
      {"NoiseParberry", "Primitive/Coherent"},
      {"NoisePingpong", "Primitive/Coherent"},
      {"NoiseRidged", "Primitive/Coherent"},
      {"NoiseSwiss", "Primitive/Coherent"},
      {"NormalDisplacement", "Filter/Recast"},
      {"Opening", "Operator/Morphology"},
      {"Paraboloid", "Primitive/Function"},
      {"Path", "Geometry/Path"},
      {"PathBezier", "Geometry/Path"},
      {"PathBezierRound", "Geometry/Path"},
      {"PathBspline", "Geometry/Path"},
      {"PathDecasteljau", "Geometry/Path"},
      {"PathDig", "Geometry/Path"},
      {"PathFind", "Geometry/Path"},
      {"PathFractalize", "Geometry/Path"},
      {"PathFromCsv", "Geometry/Path"},
      {"PathMeanderize", "Geometry/Path"},
      {"PathResample", "Geometry/Path"},
      {"PathSDF", "Geometry/Path"},
      {"PathShuffle", "Geometry/Path"},
      {"PathSmooth", "Geometry/Path"},
      {"PathToCloud", "Geometry/Path"},
      {"PathToHeightmap", "Geometry/Path"},
      {"Plateau", "Filter/Recurve"},
      {"PolygonField", "Primitive/Coherent"},
      {"PolygonFieldFbm", "Primitive/Coherent"},
      {"Preview", "Debug"},
      {"QuiltingBlend", "Operator/Resynthesis"},
      {"QuiltingExpand", "Operator/Resynthesis"},
      {"QuiltingShuffle", "Operator/Resynthesis"},
      {"RadialDisplacementToXy", "Math"},
      {"RecastCanyon", "Filter/Recast"},
      {"RecastCliff", "Filter/Recast"},
      {"RecastCliffDirectional", "Filter/Recast"},
      {"RecastCracks", "Filter/Recast"},
      {"RecastSag", "Filter/Recast"},
      {"Receive", "Routing"},
      {"Recurve", "Filter/Recurve"},
      {"RecurveKura", "Filter/Recurve"},
      {"RecurveS", "Filter/Recurve"},
      {"RelativeDistanceFromSkeleton", "Operator/Morphology"},
      {"RelativeElevation", "Features/Landform"},
      {"Remap", "Filter/Range"},
      {"Rescale", "Filter/Range"},
      {"Reverse", "Math/Base"},
      {"ReverseAboveThreshold", "Filter/Recurve"},
      {"ReverseMidpoint", "WIP"}, // Primitive/Authoring
      {"Ridgelines", "Primitive/Authoring"},
      {"Rift", "Primitive/Function"},
      {"Rifts", "Erosion/Hydraulic"},
      {"Rotate", "Operator/Transform"},
      {"Ruggedness", "Features"},
      {"Rugosity", "Features"},
      {"Saturate", "Filter/Recurve"},
      {"ScanMask", "Mask"},
      {"SedimentDeposition", "WIP"}, // "Erosion/Deposition"
      {"SelectAngle", "Mask/Selector"},
      {"SelectBlobLog", "Mask/Selector"},
      {"SelectCavities", "Mask/Selector"},
      {"SelectGt", "Mask/Selector"},
      {"SelectInterval", "Mask/Selector"},
      {"SelectInwardOutward", "Mask/Selector"},
      {"SelectMidrange", "Mask/Selector"},
      {"SelectMultiband3", "Mask/Selector"},
      {"SelectPulse", "Mask/Selector"},
      {"SelectRivers", "Mask/Selector"},
      {"SelectSlope", "Mask/Selector"},
      {"SelectTransitions", "Mask/Selector"},
      {"SelectValley", "Mask/Selector"},
      {"SetAlpha", "Texture"},
      {"SetBorders", "Math/Boundaries"},
      {"ShapeIndex", "Filter/Smoothing"},
      {"SharpenCone", "Filter/Smoothing"},
      {"ShiftElevation", "Filter/Range"},
      {"Skeleton", "Operator/Morphology"},
      {"Slope", "Primitive/Function"},
      {"SmoothCpulse", "Filter/Smoothing"},
      {"SmoothFill", "Filter/Smoothing"},
      {"SmoothFillHoles", "Filter/Smoothing"},
      {"SmoothFillSmearPeaks", "Filter/Smoothing"},
      {"Smoothstep", "Math/Base"},
      {"Stamping", "Primitive/Coherent"},
      {"StdLocal", "Features"},
      {"SteepenConvective", "Filter/Recast"},
      {"Step", "Primitive/Function"},
      {"Strata", "Erosion/Stratify"},
      {"Stratify", "Erosion/Stratify"},
      {"StratifyOblique", "Erosion/Stratify"},
      {"StratifyMultiscale", "Erosion/Stratify"},
      {"Terrace", "Filter/Recurve"},
      {"TextureQuiltingExpand", "Texture"},
      {"TextureQuiltingShuffle", "Texture"},
      {"TextureUvChecker", "Texture"},
      {"Thermal", "Erosion/Thermal"},
      {"ThermalAutoBedrock", "Erosion/Thermal"},
      {"ThermalFlatten", "WIP"}, // "Erosion/Thermal"
      {"ThermalInflate", "Erosion/Thermal"},
      {"ThermalRib", "WIP"}, // "Erosion/Thermal"
      {"ThermalRidge", "Erosion/Thermal"},
      {"ThermalSchott", "WIP"}, // "Erosion/Thermal"
      {"ThermalScree", "Erosion/Thermal"},
      {"Thru", "Routing"},
      {"Toggle", "Routing"},
      {"Translate", "Operator/Transform"},
      {"Unsphericity", "Features/Landform"},
      {"ValleyWidth", "Features/Landform"},
      {"Vorolines", "Primitive/Coherent"},
      {"VorolinesFbm", "Primitive/Coherent"},
      {"Voronoi", "Primitive/Coherent"},
      {"VoronoiFbm", "Primitive/Coherent"},
      {"Voronoise", "Primitive/Coherent"},
      {"Vororand", "Primitive/Coherent"},
      {"Warp", "Operator/Transform"},
      {"WarpDownslope", "WIP"}, // Operator/Transform
      {"WaveDune", "Primitive/Function"},
      {"WaveSine", "Primitive/Function"},
      {"WaveSquare", "Primitive/Function"},
      {"WaveTriangular", "Primitive/Function"},
      {"White", "Primitive/Random"},
      {"WhiteDensityMap", "Primitive/Random"},
      {"WhiteSparse", "Primitive/Random"},
      {"Wrinkle", "WIP"}, // Filter/Recast
      {"ZeroedEdges", "Math/Boundaries"},
      {"Zoom", "Operator/Transform"},
      {"ZScore", "Features"},
  };

  return node_inventory;
} // namespace hesiod

std::shared_ptr<gnode::Node> node_factory(const std::string           &node_type,
                                          std::shared_ptr<ModelConfig> config)
{
  // --- specialized nodes

  if (node_type == "Broadcast")
  {
    auto sptr = std::make_shared<hesiod::BroadcastNode>(node_type, config);
    setup_broadcast_node(sptr.get());
    sptr->set_compute_fct(&compute_broadcast_node);
    return sptr;
  }
  else if (node_type == "Receive")
  {
    auto sptr = std::make_shared<hesiod::ReceiveNode>(node_type, config);
    setup_receive_node(sptr.get());
    sptr->set_compute_fct(&compute_receive_node);
    return sptr;
  }

  // --- generic nodes

  auto sptr = std::make_shared<hesiod::BaseNode>(node_type, config);

  switch (str2int(node_type.c_str()))
  {
    SETUP_NODE(Abs, abs);
    SETUP_NODE(AbsSmooth, abs_smooth);
    SETUP_NODE(AccumulationCurvature, accumulation_curvature);
    SETUP_NODE(BasaltField, basalt_field);
    SETUP_NODE(Blend, blend);
    SETUP_NODE(BlendPoissonBf, blend_poisson_bf);
    SETUP_NODE(Border, border);
    SETUP_NODE(Brush, brush);
    SETUP_NODE(Bump, bump);
    SETUP_NODE(BumpLorentzian, bump_lorentzian);
    SETUP_NODE(Caldera, caldera);
    SETUP_NODE(Clamp, clamp);
    SETUP_NODE(Closing, closing);
    SETUP_NODE(Cloud, cloud);
    SETUP_NODE(CloudFromCsv, cloud_from_csv);
    SETUP_NODE(CloudMerge, cloud_merge);
    SETUP_NODE(CloudLattice, cloud_lattice);
    SETUP_NODE(CloudRandom, cloud_random);
    SETUP_NODE(CloudRandomDensity, cloud_random_density);
    SETUP_NODE(CloudRandomDistance, cloud_random_distance);
    SETUP_NODE(CloudRandomPowerLaw, cloud_random_power_law);
    SETUP_NODE(CloudRandomWeibull, cloud_random_weibull);
    SETUP_NODE(CloudRemapValues, cloud_remap_values);
    SETUP_NODE(CloudSDF, cloud_sdf);
    SETUP_NODE(CloudSetValuesFromBorderDistance, cloud_set_values_from_border_distance);
    SETUP_NODE(CloudSetValuesFromHeightmap, cloud_set_values_from_heightmap);
    SETUP_NODE(CloudSetValuesFromMinDistance, cloud_set_values_from_min_distance);
    SETUP_NODE(CloudShuffle, cloud_shuffle);
    SETUP_NODE(CloudToArrayInterp, cloud_to_array_interp);
    SETUP_NODE(CloudToPath, cloud_to_path);
    SETUP_NODE(ColorizeCmap, colorize_cmap);
    SETUP_NODE(ColorizeGradient, colorize_gradient);
    SETUP_NODE(ColorizeSolid, colorize_solid);
    SETUP_NODE(CombineMask, combine_mask);
    SETUP_NODE(Constant, constant);
    SETUP_NODE(Cos, cos);
    SETUP_NODE(ConvolveSVD, convolve_svd);
    SETUP_NODE(Crater, crater);
    SETUP_NODE(Debug, debug);
    SETUP_NODE(Dendry, dendry);
    SETUP_NODE(DepressionFilling, depression_filling);
    SETUP_NODE(Detrend, detrend);
    SETUP_NODE(DiffusionLimitedAggregation, diffusion_limited_aggregation);
    SETUP_NODE(Dilation, dilation);
    SETUP_NODE(DirectionalBlur, directional_blur);
    SETUP_NODE(DistanceTransform, distance_transform);
    SETUP_NODE(Erosion, erosion);
    SETUP_NODE(ExpandShrink, expand_shrink);
    SETUP_NODE(ExportAsset, export_asset);
    SETUP_NODE(ExportAsCubemap, export_as_cubemap);
    SETUP_NODE(ExportCloud, export_cloud);
    SETUP_NODE(ExportHeightmap, export_heightmap);
    SETUP_NODE(ExportNormalMap, export_normal_map);
    SETUP_NODE(ExportPath, export_path);
    SETUP_NODE(ExportTexture, export_texture);
    SETUP_NODE(Falloff, falloff);
    SETUP_NODE(FillTalus, fill_talus);
    SETUP_NODE(FloodingFromBoundaries, flooding_from_boundaries);
    SETUP_NODE(FloodingFromPoint, flooding_from_point);
    SETUP_NODE(FloodingUniformLevel, flooding_uniform_level);
    SETUP_NODE(FlowStream, flow_stream);
    SETUP_NODE(Fold, fold);
    SETUP_NODE(GaborWaveFbm, gabor_wave_fbm);
    SETUP_NODE(Gain, gain);
    SETUP_NODE(GammaCorrection, gamma_correction);
    SETUP_NODE(GammaCorrectionLocal, gamma_correction_local);
    SETUP_NODE(GaussianDecay, gaussian_decay);
    SETUP_NODE(GaussianPulse, gaussian_pulse);
    SETUP_NODE(Gavoronoise, gavoronoise);
    SETUP_NODE(Gradient, gradient);
    SETUP_NODE(GradientAngle, gradient_angle);
    SETUP_NODE(GradientNorm, gradient_norm);
    SETUP_NODE(GradientTalus, gradient_talus);
    SETUP_NODE(HeightmapToKernel, heightmap_to_kernel);
    SETUP_NODE(HeightmapToMask, heightmap_to_mask);
    SETUP_NODE(HeightmapToNormalMap, heightmap_to_normal_map);
    SETUP_NODE(HeightmapToRGBA, heightmap_to_rgba);
    SETUP_NODE(HydraulicBlur, hydraulic_blur);
    SETUP_NODE(HydraulicMusgrave, hydraulic_musgrave);
    SETUP_NODE(HydraulicParticle, hydraulic_particle);
    SETUP_NODE(HydraulicProcedural, hydraulic_procedural);
    SETUP_NODE(HydraulicSchott, hydraulic_schott);
    SETUP_NODE(HydraulicStream, hydraulic_stream);
    SETUP_NODE(HydraulicStreamLog, hydraulic_stream_log);
    SETUP_NODE(HydraulicStreamUpscaleAmplification,
               hydraulic_stream_upscale_amplification);
    SETUP_NODE(HydraulicVpipes, hydraulic_vpipes);
    SETUP_NODE(ImportHeightmap, import_heightmap);
    SETUP_NODE(ImportTexture, import_texture);
    SETUP_NODE(Inverse, inverse);
    SETUP_NODE(KernelDiskSmooth, kernel_disk_smooth);
    SETUP_NODE(KernelGabor, kernel_gabor);
    SETUP_NODE(KernelPrim, kernel_prim);
    SETUP_NODE(KmeansClustering2, kmeans_clustering2);
    SETUP_NODE(KmeansClustering3, kmeans_clustering3);
    SETUP_NODE(Kuwahara, kuwahara);
    SETUP_NODE(Laplace, laplace);
    SETUP_NODE(Lerp, lerp);
    SETUP_NODE(MakeBinary, make_binary);
    SETUP_NODE(MakePeriodic, make_periodic);
    SETUP_NODE(MakePeriodicStitching, make_periodic_stitching);
    SETUP_NODE(MeanShift, mean_shift);
    SETUP_NODE(Median3x3, median3x3);
    SETUP_NODE(MedianPseudo, median_pseudo);
    SETUP_NODE(Mixer, mixer);
    SETUP_NODE(MixNormalMap, mix_normal_map);
    SETUP_NODE(MixTexture, mix_texture);
    SETUP_NODE(MorphologicalGradient, morphological_gradient);
    SETUP_NODE(MorphologicalTopHat, morphological_top_hat);
    SETUP_NODE(MountainRangeRadial, mountain_range_radial);
    SETUP_NODE(Noise, noise);
    SETUP_NODE(NoiseFbm, noise_fbm);
    SETUP_NODE(NoiseIq, noise_iq);
    SETUP_NODE(NoiseJordan, noise_iq);
    SETUP_NODE(NoiseParberry, noise_parberry);
    SETUP_NODE(NoisePingpong, noise_pingpong);
    SETUP_NODE(NoiseRidged, noise_ridged);
    SETUP_NODE(NoiseSwiss, noise_swiss);
    SETUP_NODE(NormalDisplacement, normal_displacement);
    SETUP_NODE(Opening, opening);
    SETUP_NODE(Paraboloid, paraboloid);
    SETUP_NODE(Path, path);
    SETUP_NODE(PathBezier, path_bezier);
    SETUP_NODE(PathBezierRound, path_bezier_round);
    SETUP_NODE(PathBspline, path_bspline);
    SETUP_NODE(PathDecasteljau, path_decasteljau);
    SETUP_NODE(PathDig, path_dig);
    SETUP_NODE(PathFind, path_find);
    SETUP_NODE(PathFractalize, path_fractalize);
    SETUP_NODE(PathFromCsv, path_from_csv);
    SETUP_NODE(PathMeanderize, path_meanderize);
    SETUP_NODE(PathResample, path_resample);
    SETUP_NODE(PathSDF, path_sdf);
    SETUP_NODE(PathShuffle, path_shuffle);
    SETUP_NODE(PathSmooth, path_smooth);
    SETUP_NODE(PathToCloud, path_to_cloud);
    SETUP_NODE(PathToHeightmap, path_to_heightmap);
    SETUP_NODE(Plateau, plateau);
    SETUP_NODE(PolygonField, polygon_field);
    SETUP_NODE(PolygonFieldFbm, polygon_field_fbm);
    SETUP_NODE(Preview, preview);
    SETUP_NODE(QuiltingBlend, quilting_blend);
    SETUP_NODE(QuiltingExpand, quilting_expand);
    SETUP_NODE(QuiltingShuffle, quilting_shuffle);
    SETUP_NODE(RadialDisplacementToXy, radial_displacement_to_xy);
    SETUP_NODE(RecastCanyon, recast_canyon);
    SETUP_NODE(RecastCliff, recast_cliff);
    SETUP_NODE(RecastCliffDirectional, recast_cliff_directional);
    SETUP_NODE(RecastCracks, recast_cracks);
    SETUP_NODE(RecastSag, recast_sag);
    SETUP_NODE(Recurve, recurve);
    SETUP_NODE(RecurveKura, recurve_kura);
    SETUP_NODE(RecurveS, recurve_s);
    SETUP_NODE(RelativeDistanceFromSkeleton, relative_distance_from_skeleton);
    SETUP_NODE(RelativeElevation, relative_elevation);
    SETUP_NODE(Remap, remap);
    SETUP_NODE(Rescale, rescale);
    SETUP_NODE(Reverse, reverse);
    SETUP_NODE(ReverseAboveThreshold, reverse_above_theshold);
    SETUP_NODE(ReverseMidpoint, reverse_midpoint);
    SETUP_NODE(Ridgelines, ridgelines);
    SETUP_NODE(Rift, rift);
    SETUP_NODE(Rifts, rifts);
    SETUP_NODE(Rotate, rotate);
    SETUP_NODE(Ruggedness, ruggedness);
    SETUP_NODE(Rugosity, rugosity);
    SETUP_NODE(Saturate, saturate);
    SETUP_NODE(ScanMask, scan_mask);
    SETUP_NODE(SedimentDeposition, sediment_deposition);
    SETUP_NODE(SelectAngle, select_angle);
    SETUP_NODE(SelectBlobLog, select_blob_log);
    SETUP_NODE(SelectCavities, select_cavities);
    SETUP_NODE(SelectGt, select_gt);
    SETUP_NODE(SelectInterval, select_interval);
    SETUP_NODE(SelectInwardOutward, select_inward_outward);
    SETUP_NODE(SelectMultiband3, select_multiband3);
    SETUP_NODE(SelectMidrange, select_midrange);
    SETUP_NODE(SelectPulse, select_pulse);
    SETUP_NODE(SelectRivers, select_rivers);
    SETUP_NODE(SelectSlope, select_slope);
    SETUP_NODE(SelectTransitions, select_transitions);
    SETUP_NODE(SelectValley, select_valley);
    SETUP_NODE(SetAlpha, set_alpha);
    SETUP_NODE(SetBorders, set_borders);
    SETUP_NODE(ShapeIndex, shape_index);
    SETUP_NODE(SharpenCone, sharpen_cone);
    SETUP_NODE(ShiftElevation, shift_elevation);
    SETUP_NODE(Skeleton, skeleton);
    SETUP_NODE(Slope, slope);
    SETUP_NODE(SmoothCpulse, smooth_cpulse);
    SETUP_NODE(SmoothFill, smooth_fill);
    SETUP_NODE(SmoothFillHoles, smooth_fill_holes);
    SETUP_NODE(SmoothFillSmearPeaks, smooth_fill_smear_peaks);
    SETUP_NODE(Smoothstep, smoothstep);
    SETUP_NODE(Stamping, stamping);
    SETUP_NODE(StdLocal, std_local);
    SETUP_NODE(SteepenConvective, steepen_convective);
    SETUP_NODE(Step, step);
    SETUP_NODE(Strata, strata);
    SETUP_NODE(Stratify, stratify);
    SETUP_NODE(StratifyOblique, stratify_oblique);
    SETUP_NODE(StratifyMultiscale, stratify_multiscale);
    SETUP_NODE(Terrace, terrace);
    SETUP_NODE(TextureQuiltingExpand, texture_quilting_expand);
    SETUP_NODE(TextureQuiltingShuffle, texture_quilting_shuffle);
    SETUP_NODE(TextureUvChecker, texture_uv_checker);
    SETUP_NODE(Translate, translate);
    SETUP_NODE(Thermal, thermal);
    SETUP_NODE(ThermalAutoBedrock, thermal_auto_bedrock);
    SETUP_NODE(ThermalFlatten, thermal_flatten);
    SETUP_NODE(ThermalInflate, thermal_inflate);
    SETUP_NODE(ThermalRib, thermal_rib);
    SETUP_NODE(ThermalRidge, thermal_ridge);
    SETUP_NODE(ThermalSchott, thermal_schott);
    SETUP_NODE(ThermalScree, thermal_scree);
    SETUP_NODE(Thru, thru);
    SETUP_NODE(Toggle, toggle);
    SETUP_NODE(Unsphericity, unsphericity);
    SETUP_NODE(ValleyWidth, valley_width);
    SETUP_NODE(Vorolines, vorolines);
    SETUP_NODE(VorolinesFbm, vorolines_fbm);
    SETUP_NODE(Voronoi, voronoi);
    SETUP_NODE(VoronoiFbm, voronoi_fbm);
    SETUP_NODE(Voronoise, voronoise);
    SETUP_NODE(Vororand, vororand);
    SETUP_NODE(Warp, warp);
    SETUP_NODE(WarpDownslope, warp_downslope);
    SETUP_NODE(WaveDune, wave_dune);
    SETUP_NODE(WaveSine, wave_sine);
    SETUP_NODE(WaveSquare, wave_square);
    SETUP_NODE(WaveTriangular, wave_triangular);
    SETUP_NODE(White, white);
    SETUP_NODE(WhiteDensityMap, white_density_map);
    SETUP_NODE(WhiteSparse, white_sparse);
    SETUP_NODE(Wrinkle, wrinkle);
    SETUP_NODE(ZeroedEdges, zeroed_edges);
    SETUP_NODE(Zoom, zoom);
    SETUP_NODE(ZScore, z_score);
  default:
    throw std::invalid_argument("Unknown node type in node_factory: " + node_type);
  }

  return sptr;
}

} // namespace hesiod
