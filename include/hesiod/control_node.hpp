/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <string>

#include "gnode.hpp"
#include "highmap.hpp"

// clang-format off
#define DEFAULT_KW {2.f, 2.f}
#define DEFAULT_SEED 1
// clang-format on

namespace hesiod::cnode
{

// define hesiod's own data types
enum dtype : int
{
  dHeightMap
};

static const std::map<std::string, std::string> category_mapping = {
    {"GradientNorm", "Math/Gradient"},
    {"GradientTalus", "Math/Gradient"},
    {"GammaCorrection", "Filter/Recurve"},
    {"HydraulicParticle", "Erosion/Hydraulic"},
    {"Perlin", "Primitive/Coherent Noise"},
    {"Remap", "Filter/Range"},
    {"SmoothCpulse", "Filter/Smoothing"},
    {"WhiteDensityMap", "Primitive/Random"}};

//----------------------------------------
// Generic nodes
//----------------------------------------

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

//----------------------------------------
// End-user nodes

//----------------------------------------

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

class GammaCorrection : public Filter
{
public:
  GammaCorrection(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float gamma = 1.f;
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
  int   nparticles = 10000;
  int   c_radius = 0;
  float c_capacity = 10.f;
  float c_erosion = 0.05f;
  float c_deposition = 0.01f;
  float drag_rate = 0.01f;
  float evap_rate = 0.001f;
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
  hmap::Vec2<float> kw = DEFAULT_KW;
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

class SmoothCpulse : public Filter
{
public:
  SmoothCpulse(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  int ir = 8;
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

} // namespace hesiod::cnode
