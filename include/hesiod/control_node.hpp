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

  hmap::Vec2<int> get_shape();

  float get_vmin();

  float get_vmax();

  void set_shape(hmap::Vec2<int> new_shape);

  void set_tiling(hmap::Vec2<int> new_tiling);

  void set_vmax(float new_vmax);

  void set_vmin(float new_vmin);

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
  float           vmin = 0.f;
  float           vmax = 1.f;
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

  float get_gamma();

  void set_gamma(float new_gamma);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

private:
  float gamma = 1.f;
};

class HydraulicParticle : public Erosion
{
public:
  HydraulicParticle(std::string id);

  uint get_seed();

  int get_nparticles();

  int get_c_radius();

  float get_c_capacity();

  float get_c_erosion();

  float get_c_deposition();

  float get_drag_rate();

  float get_evap_rate();

  void set_seed(uint new_seed);

  void set_nparticles(int new_nparticles);

  void set_c_radius(int new_c_radius);

  void set_c_capacity(float new_c_capacity);

  void set_c_erosion(float new_c_erosion);

  void set_c_deposition(float new_c_deposition);

  void set_drag_rate(float new_drag_rate);

  void set_evap_rate(float new_evap_rate);

  void compute_erosion(hmap::HeightMap &h,
                       hmap::HeightMap *p_bedrock,
                       hmap::HeightMap *p_moisture_map,
                       hmap::HeightMap *p_mask,
                       hmap::HeightMap *p_erosion_map,
                       hmap::HeightMap *p_deposition_map);

private:
  uint  seed = 1;
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

  hmap::Vec2<float> get_kw();

  uint get_seed();

  void set_kw(hmap::Vec2<float> new_kw);

  void set_seed(uint new_seed);

  void compute();

private:
  hmap::Vec2<float> kw = DEFAULT_KW;
  uint              seed = DEFAULT_SEED;
};

class Remap : public Unary
{
public:
  Remap(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  float get_vmin();

  float get_vmax();

  void set_vmin(float new_vmin);

  void set_vmax(float new_vmax);

private:
  float vmin = 0.f;
  float vmax = 1.f;
};

class SmoothCpulse : public Filter
{
public:
  SmoothCpulse(std::string id);

  int get_ir();

  void set_ir(float new_ir);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

private:
  int ir = 8;
};

class WhiteDensityMap : public gnode::Node
{
public:
  WhiteDensityMap(std::string id);

  uint get_seed();

  void set_seed(uint new_seed);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();

private:
  hmap::Vec2<int> shape = {0, 0};
  uint            seed = DEFAULT_SEED;
};

} // namespace hesiod::cnode
