/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <string>

#include "gnode.hpp"
#include "highmap.hpp"

// clang-format off
#define DEFAULT_KERNEL_SHAPE {17, 17}
#define DEFAULT_KW 2.f
#define DEFAULT_SEED 1
// clang-format on

namespace hesiod::cnode
{

// define hesiod's own data types
enum dtype : int
{
  dArray,
  dCloud,
  dHeightMap,
  dHeightMapRGB,
  dPath
};

enum blending_method : int
{
  add,
  exclusion,
  gradients,
  maximum,
  maximum_smooth,
  minimum,
  minimum_smooth,
  multiply,
  multiply_add,
  negate,
  overlay,
  soft,
  substract
};

enum export_type : int
{
  binary,
  png8bit,
  png16bit,
  raw16bit
};

enum kernel : int
{
  cone,
  cubic_pulse,
  lorentzian,
  smooth_cosine
};

//----------------------------------------
// Node types / categories
//----------------------------------------

static const std::map<std::string, std::string> category_mapping = {
    // {"AlterElevation", "Operator/Transform"}, // BROKEN
    {"BaseElevation", "Primitive/Manual"},
    {"BezierPath", "Geometry/Path"},
    {"BiquadPulse", "Primitive/Function"},
    {"Blend", "Operator/Blend"},
    {"Brush", "Primitive/Manual"},
    {"Bump", "Primitive/Function"},
    {"Caldera", "Primitive/Geological"},
    {"Checkerboard", "Primitive/Coherent Noise"},
    {"Clamp", "Filter/Range"},
    {"Clone", "Routing"},
    {"Cloud", "Geometry/Cloud"},
    {"CloudToArrayInterp", "Primitive/Manual"},
    {"Colorize", "Texture"},
    {"PreviewColorize", "Texture"},
    {"ConvolveSVD", "Math/Convolution"},
    // {"CubicPulseTruncated", "Primitive/Kernel"}, // useless
    {"Debug", "Debug"},
    {"DepressionFilling", "Erosion"}, // not distributed
    {"DigPath", "Roads"},             // partially distributed
    {"DistanceTransform", "Math"},
    {"Equalize", "Filter/Recurve"}, // not distributed
    {"ErosionMaps", "Erosion/Hydraulic"},
    {"ExpandShrink", "Filter/Recast"},
    {"ExpandShrinkDirectional", "Filter/Recast"},
    {"Export", "IO/Files"},
    {"ExportRGB", "IO/Files"},
    {"FbmPerlin", "Primitive/Coherent Noise"},
    {"FbmSimplex", "Primitive/Coherent Noise"},
    {"FbmWorley", "Primitive/Coherent Noise"},
    {"FractalizePath", "Geometry/Path"},
    {"GaborNoise", "Primitive/Coherent Noise"},
    {"Gain", "Filter/Recurve"},
    {"GammaCorrection", "Filter/Recurve"},
    {"GammaCorrectionLocal", "Filter/Recurve"},
    {"GaussianPulse", "Primitive/Function"},
    {"Gradient", "Math/Gradient"},
    {"GradientAngle", "Math/Gradient"},
    {"GradientNorm", "Math/Gradient"},
    {"GradientTalus", "Math/Gradient"},
    {"HydraulicAlgebric", "Erosion/Hydraulic"},
    // {"HydraulicBenes", "Erosion/Hydraulic"}, // BROKEN
    {"HydraulicParticle", "Erosion/Hydraulic"},
    {"HydraulicRidge", "Erosion/Hydraulic"}, // not distributed
    {"HydraulicStream", "Erosion/Hydraulic"},
    {"HydraulicVpipes", "Erosion/Hydraulic"},
    {"Import", "IO/Files"},
    {"Kernel", "Primitive/Kernel"},
    {"KmeansClustering2", "Features"}, // not distributed
    {"KmeansClustering3", "Features"}, // not distributed
    {"Laplace", "Filter/Smoothing"},
    {"LaplaceEdgePreserving", "Filter/Smoothing"},
    {"Lerp", "Operator/Blend"},
    {"MakeBinary", "Filter/Recurve"},
    {"MeanderizePath", "Geometry/Path"},
    {"MeanLocal", "Filter/Smoothing"},
    {"Median3x3", "Filter/Smoothing"},
    {"MinimumLocal", "Filter/Smoothing"},
    {"MixRGB", "Texture"},
    {"NormalDisplacement", "Filter/Recast"},
    {"OneMinus", "Math/Base"},
    {"Path", "Geometry/Path"},
    {"PathFinding", "Roads"},
    {"PathToHeightmap", "Geometry/Path"},
    {"Perlin", "Primitive/Coherent Noise"},
    {"PerlinBillow", "Primitive/Coherent Noise"},
    {"Plateau", "Filter/Recurve"},
    {"Preview", "Debug"},
    {"RecastCanyon", "Filter/Recast"},
    {"Recurve", "Filter/Recurve"},
    {"RecurveKura", "Filter/Recurve"},
    {"RecurveS", "Filter/Recurve"},
    {"RelativeElevation", "Features"},
    {"Remap", "Filter/Range"},
    {"RidgedPerlin", "Primitive/Coherent Noise"},
    {"Rugosity", "Features"},
    {"SedimentDeposition", "Erosion/Thermal"},
    {"SelectCavities", "Mask"},
    {"SelectEq", "Mask"},
    {"SelectInterval", "Mask"},
    {"SelectRivers", "Mask"}, // not distributed
    {"SelectTransitions", "Mask"},
    {"Simplex", "Primitive/Coherent Noise"},
    {"Slope", "Primitive/Function"},
    {"SmoothCpulse", "Filter/Smoothing"},
    {"SmoothFill", "Filter/Smoothing"},
    {"SmoothFillHoles", "Filter/Smoothing"},
    {"SmoothFillSmearPeaks", "Filter/Smoothing"},
    {"SteepenConvective", "Filter/Recast"},
    {"Step", "Primitive/Function"},
    {"StratifyMultiscale", "Erosion/Stratify"},
    {"StratifyOblique", "Erosion/Stratify"},
    {"Thermal", "Erosion/Thermal"},
    {"ThermalAutoBedrock", "Erosion/Thermal"},
    {"ThermalScree", "Erosion/Thermal"},
    {"ToMask", "Mask"},
    {"ValleyWidth", "Features"},
    {"ValueNoiseDelaunay", "Primitive/Coherent Noise"},
    {"ValueNoiseLinear", "Primitive/Coherent Noise"},
    {"ValueNoiseThinplate", "Primitive/Coherent Noise"},
    {"Warp", "Operator/Transform"},
    {"WarpDownslope", "Operator/Transform"},
    {"WaveDune", "Primitive/Function"},
    {"WaveSine", "Primitive/Function"},
    {"White", "Primitive/Random"},
    {"WhiteDensityMap", "Primitive/Random"},
    {"WhiteSparse", "Primitive/Random"},
    {"Worley", "Primitive/Coherent Noise"},
    {"WorleyDouble", "Primitive/Coherent Noise"},
    {"WorleyValue", "Primitive/Coherent Noise"},
    {"ZeroedEdges", "Math/Boundaries"}};

//----------------------------------------
// Generic nodes
//----------------------------------------

class Unary : public gnode::Node // most basic, 1 in / 1 out
{
public:
  Unary(std::string id);

  void update_inner_bindings();

  void compute();

  virtual void compute_in_out(hmap::HeightMap &, hmap::HeightMap *)
  {
    LOG_ERROR("Compute not defined for generic in/out [%s])", this->id.c_str());
    throw std::runtime_error("undefined 'compute_in_out' method");
  }

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Binary : public gnode::Node // basic, 2 in / 1 out
{
public:
  Binary(std::string id);

  void update_inner_bindings();

  void compute();

  virtual void compute_in_out(hmap::HeightMap &,
                              hmap::HeightMap *,
                              hmap::HeightMap *)
  {
    LOG_ERROR("Compute not defined for generic in/out [%s])", this->id.c_str());
    throw std::runtime_error("undefined 'compute_in_out' method");
  }

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Debug : public gnode::Node
{
public:
  Debug(std::string id);

  void compute();
};

class Erosion : public gnode::Node
{
public:
  Erosion(std::string id);

  void update_inner_bindings();

  void compute();

  virtual void compute_erosion(hmap::HeightMap &, // eroded hmap
                               hmap::HeightMap *, // bedrock
                               hmap::HeightMap *, // moisture map
                               hmap::HeightMap *, // mask
                               hmap::HeightMap *, // erosion map
                               hmap::HeightMap *) // deposition map
  {
    LOG_ERROR("Compute not defined for generic filter [%s]", this->id.c_str());
    throw std::runtime_error("undefined 'compute_erosion' method");
  }

protected:
  hmap::HeightMap value_out = hmap::HeightMap(); // eroded heightmap
  hmap::HeightMap erosion_map = hmap::HeightMap();
  hmap::HeightMap deposition_map = hmap::HeightMap();
};

class Filter : public gnode::Node
{
public:
  Filter(std::string id);

  void update_inner_bindings();

  void compute();

  virtual void compute_filter(hmap::HeightMap &, hmap::HeightMap *)
  {
    LOG_ERROR("Compute not defined for generic filter [%s])", this->id.c_str());
    throw std::runtime_error("undefined 'compute_filter' method");
  }

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Mask : public gnode::Node
{
public:
  Mask(std::string id);

  void update_inner_bindings();

  void compute();

  virtual void compute_mask(hmap::HeightMap &, hmap::HeightMap *)
  {
    LOG_ERROR("Compute not defined for generic mask [%s])", this->id.c_str());
    throw std::runtime_error("undefined 'compute_mask' method");
  }

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  bool            normalize = true;
  bool            inverse = false;
  bool            smoothing = false;
  int             ir_smoothing = 16;
};

class Primitive : public gnode::Node
{
public:
  Primitive(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap);

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  float           vmin = 0.f;
  float           vmax = 1.f;
  bool            inverse = false;

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

//----------------------------------------
// Data routing nodes
//----------------------------------------

class Clone : public gnode::Node
{
public:
  Clone(std::string id);

  void remove_unused_outputs();

  void update_inner_bindings();

  void compute();

protected:
  int             id_count = 0;
  int             n_outputs = 1;
  hmap::HeightMap value_out = hmap::HeightMap();
};

//----------------------------------------
// End-user nodes
//----------------------------------------

class AlterElevation : public gnode::Node
{
public:
  AlterElevation(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  int             ir = 64;
  float           footprint_ratio = 1.f;
  float           vmin = 0.f;
  float           vmax = 1.f;
};

class BaseElevation : public Primitive
{
public:
  BaseElevation(std::string     id,
                hmap::Vec2<int> shape,
                hmap::Vec2<int> tiling,
                float           overlap);

  void compute();

protected:
  std::vector<std::vector<float>> values = {{0.f, 0.f, 0.f},
                                            {0.f, 0.f, 0.f},
                                            {0.f, 0.f, 0.f}};
  float                           width_factor = 1.f;
  bool                            remap = false;
};

class BezierPath : public gnode::Node
{
public:
  BezierPath(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Path value_out = hmap::Path();
  float      curvature_ratio = 0.3f;
  int        edge_divisions = 10;
};

class BiquadPulse : public Primitive
{
public:
  BiquadPulse(std::string     id,
              hmap::Vec2<int> shape,
              hmap::Vec2<int> tiling,
              float           overlap);

  void compute();

protected:
  float gain = 1.f;
};

class Blend : public Binary
{
public:
  Blend(std::string id);

  void compute_in_out(hmap::HeightMap &h_out,
                      hmap::HeightMap *p_h_in1,
                      hmap::HeightMap *p_h_in2);

protected:
  std::map<std::string, int> blending_method_map = {
      {"add", blending_method::add},
      {"exclusion", blending_method::exclusion},
      {"gradients", blending_method::gradients},
      {"maximum", blending_method::maximum},
      {"maximum_smooth", blending_method::maximum_smooth},
      {"minimum", blending_method::minimum},
      {"minimum_smooth", blending_method::minimum_smooth},
      {"multiply", blending_method::multiply},
      {"multiply_add", blending_method::multiply_add},
      {"negate", blending_method::negate},
      {"overlay", blending_method::overlay},
      {"soft", blending_method::soft},
      {"substract", blending_method::substract}};
  int   method = 0;
  float k = 0.1f; // smooth intensity for smooth min and max
  int   ir = 4;   // for gradients
};

class Bump : public Primitive
{
public:
  Bump(std::string     id,
       hmap::Vec2<int> shape,
       hmap::Vec2<int> tiling,
       float           overlap);

  void compute();

protected:
  float gain = 1.f;
};

class Brush : public gnode::Node
{
public:
  Brush(std::string     id,
        hmap::Vec2<int> shape,
        hmap::Vec2<int> tiling,
        float           overlap);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  float           vmin = 0.f;
  float           vmax = 1.f;
  bool            inverse = false;
  bool            remap = false;

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

class Checkerboard : public Primitive
{
public:
  Checkerboard(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  void compute();

protected:
  hmap::Vec2<float> kw = {DEFAULT_KW, DEFAULT_KW};
};

class Caldera : public Primitive
{
public:
  Caldera(std::string     id,
          hmap::Vec2<int> shape,
          hmap::Vec2<int> tiling,
          float           overlap);

  void compute();

protected:
  float             radius = 128.f;
  float             sigma_inner = 16.f;
  float             sigma_outer = 32.f;
  float             noise_r_amp = 32.f;
  float             z_bottom = 0.5f;
  float             noise_ratio_z = 0.1f;
  hmap::Vec2<float> center = {0.5f, 0.5f};
};

class Clamp : public Unary
{
public:
  Clamp(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  float vmin = 0.f;
  float vmax = 1.f;
  bool  smooth_min = false;
  bool  smooth_max = false;
  float k_min = 0.05f;
  float k_max = 0.05f;
};

class Cloud : public gnode::Node
{
public:
  Cloud(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Cloud value_out = hmap::Cloud();
};

class CloudToArrayInterp : public Primitive
{
public:
  CloudToArrayInterp(std::string     id,
                     hmap::Vec2<int> shape,
                     hmap::Vec2<int> tiling,
                     float           overlap);

  void compute();

protected:
  int interpolation_method = 0;
};

class Colorize : public gnode::Node
{
public:
  Colorize(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMapRGB value_out = hmap::HeightMapRGB();
  bool               reverse = false;
  bool               clamp = false;
  float              vmin = 0.f;
  float              vmax = 1.f;
};

class ConvolveSVD : public gnode::Node
{
public:
  ConvolveSVD(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  int             rank = 3;
};

class CubicPulseTruncated : public gnode::Node
{
public:
  CubicPulseTruncated(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::Array     value_out = hmap::Array();
  bool            normalized = true;
  float           slant_ratio = 0.1f;
  float           angle = 30.f;
  float           vmin = 0.f;
  float           vmax = 1.f;
  hmap::Vec2<int> shape = DEFAULT_KERNEL_SHAPE;
};

class DepressionFilling : public Unary
{
public:
  DepressionFilling(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  int   iterations = 1000;
  float epsilon = 1e-4f;
};

class DigPath : public gnode::Node
{
public:
  DigPath(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  int             width = 1;
  int             decay = 2;
  int             flattening_radius = 16;
  bool            force_downhill = false;
  float           depth = 0.f;
};

class DistanceTransform : public gnode::Node
{
public:
  DistanceTransform(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  hmap::Vec2<int> shape_working = {512, 512};
  bool            reverse = true;
  float           vmin = 0.f;
  float           vmax = 1.f;
};

class Equalize : public Filter
{
public:
  Equalize(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);
};

class ErosionMaps : public gnode::Node
{
public:
  ErosionMaps(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap erosion_map = hmap::HeightMap();
  hmap::HeightMap deposition_map = hmap::HeightMap();
  float           tolerance = 0.f;
};

class ExpandShrink : public Filter
{
public:
  ExpandShrink(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  int                        ir = 4;
  bool                       shrink = false;
  std::map<std::string, int> kernel_map = {
      {"cone", kernel::cone},
      {"cubic_pulse", kernel::cubic_pulse},
      {"lorentzian", kernel::lorentzian},
      {"smooth_cosine", kernel::smooth_cosine}};
  int kernel = kernel::cubic_pulse;
};

class ExpandShrinkDirectional : public Filter
{
public:
  ExpandShrinkDirectional(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  int   ir = 4;
  float angle = 30.f;
  float aspect_ratio = 0.2f;
  float anisotropy = 1.f;
  bool  shrink = false;
};

class Export : public gnode::Node
{
public:
  Export(std::string id);

  void compute();

  void write_file();

protected:
  bool        auto_export = false;
  int         export_format = export_type::png8bit;
  std::string fname = "export.png";
};

class ExportRGB : public gnode::Node
{
public:
  ExportRGB(std::string id);

  void compute();

  void write_file();

protected:
  bool        auto_export = false;
  std::string fname = "export_rgb.png";
};

class FbmPerlin : public Primitive
{
public:
  FbmPerlin(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap);

  void compute();

protected:
  hmap::Vec2<float> kw = {DEFAULT_KW, DEFAULT_KW};
  int               seed = DEFAULT_SEED;
  int               octaves = 8;
  float             weight = 0.7f;
  float             persistence = 0.5f;
  float             lacunarity = 2.f;
};

class FbmSimplex : public Primitive
{
public:
  FbmSimplex(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap);

  void compute();

protected:
  hmap::Vec2<float> kw = {DEFAULT_KW, DEFAULT_KW};
  int               seed = DEFAULT_SEED;
  int               octaves = 8;
  float             weight = 0.7f;
  float             persistence = 0.5f;
  float             lacunarity = 2.f;
};

class FbmWorley : public Primitive
{
public:
  FbmWorley(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap);

  void compute();

protected:
  hmap::Vec2<float> kw = {DEFAULT_KW, DEFAULT_KW};
  int               seed = DEFAULT_SEED;
  int               octaves = 8;
  float             weight = 0.7f;
  float             persistence = 0.5f;
  float             lacunarity = 2.f;
};

class FractalizePath : public gnode::Node
{
public:
  FractalizePath(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Path value_out = hmap::Path();
  int        iterations = 1;
  int        seed = DEFAULT_SEED;
  float      sigma = 0.3f;
  int        orientation = 0;
  float      persistence = 1.f;
};

class GaborNoise : public gnode::Node
{
public:
  GaborNoise(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  float           kw = 1.f;
  float           angle = 30.f;
  int             width = 128;
  float           density = 0.05f;
  int             seed = DEFAULT_SEED;
  float           vmin = 0.f;
  float           vmax = 1.f;

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

class Gain : public Filter
{
public:
  Gain(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float gain = 1.f;
};

class GammaCorrection : public Filter
{
public:
  GammaCorrection(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float gamma = 1.f;
};

class GammaCorrectionLocal : public Filter
{
public:
  GammaCorrectionLocal(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float gamma = 1.f;
  int   ir = 4;
  float k = 0.1f;
};

class GaussianPulse : public Primitive
{
public:
  GaussianPulse(std::string     id,
                hmap::Vec2<int> shape,
                hmap::Vec2<int> tiling,
                float           overlap);

  void compute();

protected:
  float             sigma = 32;
  bool              inverse = false;
  hmap::Vec2<float> center = {0.5f, 0.5f};
};

class Gradient : public gnode::Node
{
public:
  Gradient(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out_dx = hmap::HeightMap();
  hmap::HeightMap value_out_dy = hmap::HeightMap();
  bool            normalize = true;
  float           vmin_x = 0.f;
  float           vmax_x = 1.f;
  float           vmin_y = 0.f;
  float           vmax_y = 1.f;
};

class GradientAngle : public Unary
{
public:
  GradientAngle(std::string id);

  void compute_in_out(hmap::HeightMap &h, hmap::HeightMap *p_talus);
};

class GradientNorm : public Unary
{
public:
  GradientNorm(std::string id);

  void compute_in_out(hmap::HeightMap &h, hmap::HeightMap *p_talus);
};

class GradientTalus : public Unary
{
public:
  GradientTalus(std::string id);

  void compute_in_out(hmap::HeightMap &h, hmap::HeightMap *p_talus);
};

class HydraulicAlgebric : public Erosion
{
public:
  HydraulicAlgebric(std::string id);

  void compute_erosion(hmap::HeightMap &h,
                       hmap::HeightMap *p_bedrock,
                       hmap::HeightMap *p_moisture_map,
                       hmap::HeightMap *p_mask,
                       hmap::HeightMap *p_erosion_map,
                       hmap::HeightMap *p_deposition_map);

protected:
  float talus_global = 2.f;
  int   ir = 16;
  float c_erosion = 0.07f;
  float c_deposition = 0.01f;
  int   iterations = 1;
};

class HydraulicBenes : public Erosion
{
public:
  HydraulicBenes(std::string id);

  void compute_erosion(hmap::HeightMap &h,
                       hmap::HeightMap *p_bedrock,
                       hmap::HeightMap *p_moisture_map,
                       hmap::HeightMap *p_mask,
                       hmap::HeightMap *p_erosion_map,
                       hmap::HeightMap *p_deposition_map);

protected:
  int   iterations = 50;
  float c_capacity = 40.f;
  float c_erosion = 0.2f;
  float c_deposition = 0.8f;
  float water_level = 0.005f;
  float evap_rate = 0.01f;
  float rain_rate = 0.5f;
};

class HydraulicParticle : public Erosion
{
public:
  HydraulicParticle(std::string id);

  void compute_erosion(hmap::HeightMap &h,
                       hmap::HeightMap *p_bedrock,
                       hmap::HeightMap *p_moisture_map,
                       hmap::HeightMap *p_mask,
                       hmap::HeightMap *p_erosion_map,
                       hmap::HeightMap *p_deposition_map);

protected:
  int   seed = 1;
  int   nparticles = 40000;
  int   c_radius = 0;
  float c_capacity = 40.f;
  float c_erosion = 0.05f;
  float c_deposition = 0.1f;
  float drag_rate = 0.01f;
  float evap_rate = 0.001f;
};

class HydraulicRidge : public Filter
{
public:
  HydraulicRidge(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float talus_global = 16.f;
  float intensity = 0.5f;
  float erosion_factor = 1.5f;
  float smoothing_factor = 0.5f;
  float noise_ratio = 0.1f;
  int   ir = 16;
  int   seed = DEFAULT_SEED;
};

class HydraulicStream : public Erosion
{
public:
  HydraulicStream(std::string id);

  void compute_erosion(hmap::HeightMap &h,
                       hmap::HeightMap *p_bedrock,
                       hmap::HeightMap *p_moisture_map,
                       hmap::HeightMap *p_mask,
                       hmap::HeightMap *p_erosion_map,
                       hmap::HeightMap *p_deposition_map);

protected:
  float c_erosion = 0.05f;
  float talus_ref = 0.1f;
  int   ir = 1;
  float clipping_ratio = 10.f;
};

class HydraulicVpipes : public Erosion
{
public:
  HydraulicVpipes(std::string id);

  void compute_erosion(hmap::HeightMap &h,
                       hmap::HeightMap *p_bedrock,
                       hmap::HeightMap *p_moisture_map,
                       hmap::HeightMap *p_mask,
                       hmap::HeightMap *p_erosion_map,
                       hmap::HeightMap *p_deposition_map);

protected:
  int   iterations = 50;
  float water_height = 0.01f;
  float c_capacity = 0.1f;
  float c_erosion = 0.01f;
  float c_deposition = 0.01f;
  float rain_rate = 0.f;
  float evap_rate = 0.01f;
};

class Import : public gnode::Node
{
public:
  Import(std::string     id,
         hmap::Vec2<int> shape,
         hmap::Vec2<int> tiling,
         float           overlap);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  float           vmin = 0.f;
  float           vmax = 1.f;
  std::string     fname = "";

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

class Kernel : public gnode::Node
{
public:
  Kernel(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::Array     value_out = hmap::Array();
  bool            normalized = true;
  float           vmin = 0.f;
  float           vmax = 1.f;
  hmap::Vec2<int> shape = DEFAULT_KERNEL_SHAPE;
  int             kernel = kernel::cubic_pulse;

  std::map<std::string, int> kernel_map = {
      {"cone", kernel::cone},
      {"cubic_pulse", kernel::cubic_pulse},
      {"lorentzian", kernel::lorentzian},
      {"smooth_cosine", kernel::smooth_cosine}};
};

class KmeansClustering2 : public Binary
{
public:
  KmeansClustering2(std::string id);

  void compute_in_out(hmap::HeightMap &h_out,
                      hmap::HeightMap *p_h_in1,
                      hmap::HeightMap *p_h_in2);

protected:
  int               nclusters = 4;
  hmap::Vec2<float> weights = {1.f, 1.f};
  int               seed = DEFAULT_SEED;
  hmap::Vec2<int>   shape_clustering = {256, 256};
  bool              normalize_inputs = true;
};

class KmeansClustering3 : public gnode::Node
{
public:
  KmeansClustering3(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap   value_out = hmap::HeightMap();
  int               nclusters = 4;
  hmap::Vec3<float> weights = {1.f, 1.f, 1.f};
  int               seed = DEFAULT_SEED;
  hmap::Vec2<int>   shape_clustering = {256, 256};
  bool              normalize_inputs = true;
};

class Laplace : public Filter
{
public:
  Laplace(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float sigma = 0.2f;
  int   iterations = 3;
};

class LaplaceEdgePreserving : public Filter
{
public:
  LaplaceEdgePreserving(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float sigma = 0.2f;
  int   iterations = 3;
  float talus_global = 10.f;
};

class Lerp : public gnode::Node
{
public:
  Lerp(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  float           t = 0.5f;
};

class MakeBinary : public Unary
{
public:
  MakeBinary(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  float threshold = 0.f;
};

class MeanderizePath : public gnode::Node
{
public:
  MeanderizePath(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Path value_out = hmap::Path();
  float      radius = 0.05f;
  float      tangent_contribution = 0.1f;
  int        iterations = 1;
  float      transition_length_ratio = 0.2f;
};

class MeanLocal : public Filter
{
public:
  MeanLocal(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  int ir = 8;
};

class Median3x3 : public Filter
{
public:
  Median3x3(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);
};

class MinimumLocal : public Unary
{
public:
  MinimumLocal(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  int ir = 4;
};

class MixRGB : public gnode::Node
{
public:
  MixRGB(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMapRGB value_out = hmap::HeightMapRGB();
  float              t = 0.5f;
};

class NormalDisplacement : public Filter
{
public:
  NormalDisplacement(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float amount = 5.f;
  int   ir = 0;
  bool  reverse = false;
};

class OneMinus : public Unary
{
public:
  OneMinus(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  float vmin = 0.f;
  float vmax = 1.f;
};

class Path : public gnode::Node
{
public:
  Path(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Path value_out = hmap::Path();
  bool       closed = false;
};

class PathFinding : public gnode::Node
{
public:
  PathFinding(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Path      value_out = hmap::Path();
  hmap::Vec2<int> wshape = {256, 256};
  float           elevation_ratio = 0.5f;
  float           distance_exponent = 1.f;
};

class PathToHeightmap : public gnode::Node
{
public:
  PathToHeightmap(std::string     id,
                  hmap::Vec2<int> shape,
                  hmap::Vec2<int> tiling,
                  float           overlap);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  bool            filled = false;
  float           vmin = 0.f;
  float           vmax = 1.f;

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

class Perlin : public Primitive
{
public:
  Perlin(std::string     id,
         hmap::Vec2<int> shape,
         hmap::Vec2<int> tiling,
         float           overlap);

  void compute();

protected:
  hmap::Vec2<float> kw = {DEFAULT_KW, DEFAULT_KW};
  int               seed = DEFAULT_SEED;
};

class PerlinBillow : public Primitive
{
public:
  PerlinBillow(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  void compute();

protected:
  hmap::Vec2<float> kw = {DEFAULT_KW, DEFAULT_KW};
  int               seed = DEFAULT_SEED;
};

class Plateau : public Filter
{
public:
  Plateau(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  int   ir = 32;
  float factor = 4.f;
};

class Preview : public gnode::Node
{
public:
  Preview(std::string id);

  void compute();

  void update_inner_bindings();
};

class PreviewColorize : public gnode::Node
{
public:
  PreviewColorize(std::string id);

  void compute();

  void update_inner_bindings();
};

class RecastCanyon : public gnode::Node
{
public:
  RecastCanyon(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  float           vcut = 0.7f;
  float           gamma = 4.f;
};

class Recurve : public Filter
{
public:
  Recurve(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  std::vector<float> curve = {0.f, 0.25f, 0.5f, 0.75f, 1.f};
};

class RecurveKura : public Filter
{
public:
  RecurveKura(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float a = 2.f;
  float b = 2.f;
};

class RecurveS : public Filter
{
public:
  RecurveS(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);
};

class RelativeElevation : public Unary
{
public:
  RelativeElevation(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  int ir = 64;
};

class Remap : public Unary
{
public:
  Remap(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  float vmin = 0.f;
  float vmax = 1.f;
};

class RidgedPerlin : public Primitive
{
public:
  RidgedPerlin(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  void compute();

protected:
  hmap::Vec2<float> kw = {DEFAULT_KW, DEFAULT_KW};
  int               seed = DEFAULT_SEED;
  int               octaves = 8;
  float             weight = 0.7f;
  float             persistence = 0.5f;
  float             lacunarity = 2.f;
};

class Rugosity : public Mask
{
public:
  Rugosity(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  int   ir = 8;
  bool  clamp_max = false;
  float vc_max = 1.f;
};

class SedimentDeposition : public gnode::Node
{
public:
  SedimentDeposition(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap(); // eroded heightmap
  hmap::HeightMap deposition_map = hmap::HeightMap();
  float           talus_global = 0.1f;
  float           max_deposition = 0.01;
  int             iterations = 5;
  int             thermal_subiterations = 10;
};

class SelectCavities : public Mask
{
public:
  SelectCavities(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  int  ir = 32;
  bool concave = true;
};

class SelectEq : public Mask
{
public:
  SelectEq(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  float value = 0.f;
};

class SelectInterval : public Mask
{
public:
  SelectInterval(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  float value1 = 0.f;
  float value2 = 1.f;
};

class SelectRivers : public Mask
{
public:
  SelectRivers(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  float talus_ref = 0.1f;
  float clipping_ratio = 50.f;
};

class SelectTransitions : public gnode::Node
{
public:
  SelectTransitions(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  bool            normalize = true;
  bool            inverse = false;
  bool            smoothing = false;
  int             ir_smoothing = 16;
};

class Simplex : public Primitive
{
public:
  Simplex(std::string     id,
          hmap::Vec2<int> shape,
          hmap::Vec2<int> tiling,
          float           overlap);

  void compute();

protected:
  hmap::Vec2<float> kw = {DEFAULT_KW, DEFAULT_KW};
  int               seed = DEFAULT_SEED;
};

class Slope : public gnode::Node
{
public:
  Slope(std::string     id,
        hmap::Vec2<int> shape,
        hmap::Vec2<int> tiling,
        float           overlap);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap   value_out = hmap::HeightMap();
  float             angle = 0.f;
  float             talus_global = 4.f;
  hmap::Vec2<float> center = {0.5f, 0.5f};

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

class SmoothCpulse : public Filter
{
public:
  SmoothCpulse(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  int ir = 8;
};

class SmoothFill : public gnode::Node
{
public:
  SmoothFill(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  hmap::HeightMap deposition_map = hmap::HeightMap();
  int             ir = 32;
  float           k = 0.01f;
};

class SmoothFillHoles : public Filter
{
public:
  SmoothFillHoles(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  int ir = 8;
};

class SmoothFillSmearPeaks : public Filter
{
public:
  SmoothFillSmearPeaks(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  int ir = 8;
};

class SteepenConvective : public Filter
{
public:
  SteepenConvective(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float angle = 0.f;
  int   iterations = 1;
  int   ir = 0;
  float dt = 0.1f;
};

class Step : public Primitive
{
public:
  Step(std::string     id,
       hmap::Vec2<int> shape,
       hmap::Vec2<int> tiling,
       float           overlap);

  void compute();

protected:
  float             angle = 0.f;
  float             talus_global = 4.f;
  hmap::Vec2<float> center = {0.5f, 0.5f};
};

class StratifyMultiscale : public gnode::Node
{
public:
  StratifyMultiscale(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap    value_out = hmap::HeightMap(); // eroded heightmap
  std::vector<int>   n_strata = {2, 3, 4};
  std::vector<float> strata_noise = {0.f, 0.f, 0.f};
  std::vector<float> gamma_list = {1.5f, 0.5f, 1.f};
  std::vector<float> gamma_noise = {0.f, 0.f, 0.f};
  int                seed = DEFAULT_SEED;
};

class StratifyOblique : public gnode::Node
{
public:
  StratifyOblique(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  int             n_strata = 3;
  float           strata_noise = 0.f;
  float           gamma = 0.7f;
  float           gamma_noise = 0.f;
  float           talus_global = 2.f;
  float           angle = 30.f;
  int             seed = DEFAULT_SEED;
};

class Thermal : public gnode::Node
{
public:
  Thermal(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap(); // eroded heightmap
  hmap::HeightMap deposition_map = hmap::HeightMap();
  float           talus_global = 0.1f;
  int             iterations = 10;
};

class ThermalAutoBedrock : public gnode::Node
{
public:
  ThermalAutoBedrock(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap(); // eroded heightmap
  hmap::HeightMap deposition_map = hmap::HeightMap();
  float           talus_global = 0.1f;
  int             iterations = 10;
};

class ThermalScree : public gnode::Node
{
public:
  ThermalScree(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap(); // eroded heightmap
  hmap::HeightMap deposition_map = hmap::HeightMap();
  float           talus_global = 3.f;
  int             seed = DEFAULT_SEED;
  float           zmax = 0.3f;
  float           zmin = -1.f;
  float           noise_ratio = 0.3f;
  float           landing_talus_ratio = 1.f;
  float           landing_width_ratio = 0.25f;
  bool            talus_constraint = true;
};

class ToMask : public Mask
{
public:
  ToMask(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);
};

class ValleyWidth : public Mask
{
public:
  ValleyWidth(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  int ir = 4;
};

class ValueNoiseDelaunay : public Primitive
{
public:
  ValueNoiseDelaunay(std::string     id,
                     hmap::Vec2<int> shape,
                     hmap::Vec2<int> tiling,
                     float           overlap);

  void compute();

protected:
  float kw = DEFAULT_KW;
  int   seed = DEFAULT_SEED;
};

class ValueNoiseLinear : public Primitive
{
public:
  ValueNoiseLinear(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap);

  void compute();

protected:
  hmap::Vec2<float> kw = {DEFAULT_KW, DEFAULT_KW};
  int               seed = DEFAULT_SEED;
};

class ValueNoiseThinplate : public Primitive
{
public:
  ValueNoiseThinplate(std::string     id,
                      hmap::Vec2<int> shape,
                      hmap::Vec2<int> tiling,
                      float           overlap);

  void compute();

protected:
  float kw = DEFAULT_KW;
  int   seed = DEFAULT_SEED;
};

class Warp : public gnode::Node
{
public:
  Warp(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  float           scale = 1.f;
};

class WarpDownslope : public Filter
{
public:
  WarpDownslope(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float amount = 5.f;
  int   ir = 0;
  bool  reverse = false;
};

class WaveDune : public Primitive
{
public:
  WaveDune(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap);

  void compute();

protected:
  float kw = DEFAULT_KW;
  float angle = 0.f;
  float xtop = 0.7f;
  float xbottom = 1.f;
  float phase_shift = 0.f;
};

class WaveSine : public Primitive
{
public:
  WaveSine(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap);

  void compute();

protected:
  float kw = DEFAULT_KW;
  float angle = 0.f;
  float phase_shift = 0.f;
};

class White : public gnode::Node
{
public:
  White(std::string     id,
        hmap::Vec2<int> shape,
        hmap::Vec2<int> tiling,
        float           overlap);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  int             seed = DEFAULT_SEED;
  float           vmin = 0.f;
  float           vmax = 1.f;

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

class WhiteDensityMap : public gnode::Node
{
public:
  WhiteDensityMap(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  int             seed = DEFAULT_SEED;
};

class WhiteSparse : public gnode::Node
{
public:
  WhiteSparse(std::string     id,
              hmap::Vec2<int> shape,
              hmap::Vec2<int> tiling,
              float           overlap);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  int             seed = DEFAULT_SEED;
  float           density = 0.1f;
  float           vmin = 0.f;
  float           vmax = 1.f;

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

class Worley : public Primitive
{
public:
  Worley(std::string     id,
         hmap::Vec2<int> shape,
         hmap::Vec2<int> tiling,
         float           overlap);

  void compute();

protected:
  hmap::Vec2<float> kw = {DEFAULT_KW, DEFAULT_KW};
  int               seed = DEFAULT_SEED;
};

class WorleyDouble : public Primitive
{
public:
  WorleyDouble(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  void compute();

protected:
  hmap::Vec2<float> kw = {DEFAULT_KW, DEFAULT_KW};
  int               seed = DEFAULT_SEED;
  float             ratio = 0.5f;
  float             k = 0.05f;
};

class WorleyValue : public Primitive
{
public:
  WorleyValue(std::string     id,
              hmap::Vec2<int> shape,
              hmap::Vec2<int> tiling,
              float           overlap);

  void compute();

protected:
  hmap::Vec2<float> kw = {DEFAULT_KW, DEFAULT_KW};
  int               seed = DEFAULT_SEED;
};

class ZeroedEdges : public gnode::Node
{
public:
  ZeroedEdges(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  float           sigma = 0.25f;
  bool            remap = false;
  float           vmin = 0.f;
  float           vmax = 1.f;
};

} // namespace hesiod::cnode
