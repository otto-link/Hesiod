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
  png8bit,
  raw
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
    {"AlterElevation", "Operator/Transform"},
    {"BaseElevation", "Primitive/Manual"},
    {"BezierPath", "Geometry/Path"},
    {"Blend", "Operator/Blend"},
    {"Bump", "Primitive/Function"},
    {"Checkerboard", "Primitive/Coherent Noise"},
    {"Clamp", "Filter/Range"},
    {"Clone", "Routing"},
    {"Cloud", "Geometry/Cloud"},
    {"CloudToArrayInterp", "Primitive/Manual"},
    {"ConvolveSVD", "Math/Convolution"},
    {"Debug", "Debug"},
    {"DigPath", "Roads"},
    {"Equalize", "Filter/Recurve"},
    {"ExpandShrink", "Filter/Recast"},
    {"ExpandShrinkDirectional", "Filter/Recast"},
    {"Export", "IO/Files"},
    {"FbmPerlin", "Primitive/Coherent Noise"},
    {"FbmWorley", "Primitive/Coherent Noise"},
    {"FractalizePath", "Geometry/Path"},
    {"Gain", "Filter/Recurve"},
    {"GammaCorrection", "Filter/Recurve"},
    {"GammaCorrectionLocal", "Filter/Recurve"},
    {"GaussianPulse", "Primitive/Function"},
    {"Gradient", "Math/Gradient"},
    {"GradientNorm", "Math/Gradient"},
    {"GradientTalus", "Math/Gradient"},
    {"HydraulicParticle", "Erosion/Hydraulic"},
    {"HydraulicRidge", "Erosion/Hydraulic"},
    {"HydraulicStream", "Erosion/Hydraulic"},
    {"HydraulicVpipes", "Erosion/Hydraulic"},
    {"Import", "IO/Files"},
    {"Kernel", "Primitive/Kernel"},
    {"KmeansClustering2", "Features"},
    {"Laplace", "Filter/Smoothing"},
    {"Lerp", "Operator/Blend"},
    {"MakeBinary", "Filter/Recurve"},
    {"MeanderizePath", "Geometry/Path"},
    {"MinimumLocal", "Filter/Smoothing"},
    {"NormalDisplacement", "Filter/Recast"},
    {"OneMinus", "Math/Base"},
    {"Path", "Geometry/Path"},
    {"PathFinding", "Roads"},
    {"Perlin", "Primitive/Coherent Noise"},
    {"PerlinBillow", "Primitive/Coherent Noise"},
    {"Preview", "Debug"},
    {"RecastCanyon", "Filter/Recast"},
    {"Recurve", "Filter/Recurve"},
    {"Remap", "Filter/Range"},
    {"RidgedPerlin", "Primitive/Coherent Noise"},
    {"Rugosity", "Features"},
    {"SedimentDeposition", "Erosion/Thermal"},
    {"SelectEq", "Mask"},
    {"SelectTransitions", "Mask"},
    {"SmoothCpulse", "Filter/Smoothing"},
    {"SmoothFill", "Filter/Smoothing"},
    {"SteepenConvective", "Filter/Recast"},
    {"Step", "Primitive/Function"},
    {"Thermal", "Erosion/Thermal"},
    {"ThermalAutoBedrock", "Erosion/Thermal"},
    {"ThermalScree", "Erosion/Thermal"},
    {"ValleyWidth", "Features"},
    {"ValueNoiseDelaunay", "Primitive/Coherent Noise"},
    {"Warp", "Operator/Transform"},
    {"WarpDownslope", "Operator/Transform"},
    {"WaveSine", "Primitive/Function"},
    {"White", "Primitive/Random"},
    {"WhiteDensityMap", "Primitive/Random"},
    {"Worley", "Primitive/Coherent Noise"},
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
  bool  inverse = false;
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
  float           depth = 0.f;
};

class Equalize : public Filter
{
public:
  Equalize(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);
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
  float sigma = 32;
  bool  inverse = false;
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

private:
  hmap::Vec2<int> shape = {0, 0};
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
  float talus = 16.f / 512.f;
  float intensity = 0.5f;
  float erosion_factor = 1.5f;
  float smoothing_factor = 0.5f;
  float noise_ratio = 0.1f;
  int   ir = 0;
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
  float clipping_ratio = 10;
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
  int   iterations = 200;
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
  std::string     fname = "export.png";

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
  int             nclusters = 4;
  int             seed = DEFAULT_SEED;
  hmap::Vec2<int> shape_clustering = {256, 256};
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

class Lerp : public gnode::Node
{
public:
  Lerp(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  float           t = 0.5f;

private:
  hmap::Vec2<int> shape = {0, 0};
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

class MinimumLocal : public Unary
{
public:
  MinimumLocal(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  int ir = 4;
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

class Preview : public gnode::Node
{
public:
  Preview(std::string id);

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

class Rugosity : public Unary
{
public:
  Rugosity(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  int ir = 4;
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

class SelectEq : public Unary
{
public:
  SelectEq(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  float value = 0.f;
  bool  smoothing = false;
  int   ir = 4;
};

class SelectTransitions : public gnode::Node
{
public:
  SelectTransitions(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  bool            smoothing = false;
  int             ir = 4;
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

class SteepenConvective : public Filter
{
public:
  SteepenConvective(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float angle = 0.f;
  int   iterations = 1;
  int   ir = 0;
  float dt = 1.f;
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
  float angle = 0.f;
  float talus_global = 4.f;
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

class ValleyWidth : public Unary
{
public:
  ValleyWidth(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

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

class Warp : public gnode::Node
{
public:
  Warp(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  float           scale = 1.f;

private:
  hmap::Vec2<int> shape = {0, 0};
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

class ZeroedEdges : public gnode::Node
{
public:
  ZeroedEdges(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  float           sigma = 0.25f;
};

} // namespace hesiod::cnode
