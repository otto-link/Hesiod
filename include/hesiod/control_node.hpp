/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <string>

#include "gnode.hpp"
#include "highmap.hpp"

// clang-format off
#define DEFAULT_KW 2.f
#define DEFAULT_SEED 1
// clang-format on

namespace hesiod::cnode
{

// define hesiod's own data types
enum dtype : int
{
  dHeightMap
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
  negate,
  overlay,
  soft,
  substract
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
    {"BaseElevation", "Primitive/Manual"},
    {"Blend", "Operator/Blend"},
    {"Bump", "Primitive/Function"},
    {"Checkerboard", "Primitive/Coherent Noise"},
    {"Clamp", "Filter/Range"},
    {"Clone", "Routing"},
    {"Debug", "Debug"},
    {"ExpandShrink", "Filter/Recast"},
    {"FbmPerlin", "Primitive/Coherent Noise"},
    {"Gain", "Filter/Recurve"},
    {"GammaCorrection", "Filter/Recurve"},
    {"GammaCorrectionLocal", "Filter/Recurve"},
    {"GaussianPulse", "Primitive/Function"},
    {"Gradient", "Math/Gradient"},
    {"GradientNorm", "Math/Gradient"},
    {"GradientTalus", "Math/Gradient"},
    {"HydraulicParticle", "Erosion/Hydraulic"},
    {"KmeansClustering2", "Features"},
    {"Lerp", "Operator/Blend"},
    {"MakeBinary", "Filter/Recurve"},
    {"Perlin", "Primitive/Coherent Noise"},
    {"PerlinBillow", "Primitive/Coherent Noise"},
    {"Remap", "Filter/Range"},
    {"RidgedPerlin", "Primitive/Coherent Noise"},
    {"Rugosity", "Features"},
    {"SmoothCpulse", "Filter/Smoothing"},
    {"SteepenConvective", "Filter/Recast"},
    {"ValleyWidth", "Features"},
    {"ValueNoiseDelaunay", "Primitive/Coherent Noise"},
    {"White", "Primitive/Random"},
    {"WhiteDensityMap", "Primitive/Random"},
    {"Worley", "Primitive/Coherent Noise"}};

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

private:
  hmap::Vec2<int> shape = {0, 0};
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

private:
  hmap::Vec2<int> shape = {0, 0};
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

private:
  hmap::Vec2<int> shape = {0, 0};
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

private:
  hmap::Vec2<int> shape = {0, 0};
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

  void update_inner_bindings();

  void compute();

protected:
  int             id_count = 0;
  hmap::HeightMap value_out = hmap::HeightMap();
};

//----------------------------------------
// End-user nodes
//----------------------------------------

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
  float k_min = 0.2f;
  float k_max = 0.2f;
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
  float c_capacity = 100.f;
  float c_erosion = 0.05f;
  float c_deposition = 0.1f;
  float drag_rate = 0.01f;
  float evap_rate = 0.001f;
};

class KmeansClustering2 : public Binary
{
public:
  KmeansClustering2(std::string id);

  void compute_in_out(hmap::HeightMap &h_out,
                      hmap::HeightMap *p_h_in1,
                      hmap::HeightMap *p_h_in2);

protected:
  int nclusters = 4;
  int seed = DEFAULT_SEED;
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

class SmoothCpulse : public Filter
{
public:
  SmoothCpulse(std::string id);

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
  float dt = 1.f;
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

private:
  hmap::Vec2<int> shape = {0, 0};
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

} // namespace hesiod::cnode
