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

class Filter : public gnode::Node
{
public:
  Filter(std::string id);

  // debugging
  ~Filter()
  {
    std::cout << "Filter::~Filter() " << this->id.c_str() << "\n";
  }

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

class GammaCorrection : public Filter
{
public:
  GammaCorrection(std::string id);

  // debugging
  ~GammaCorrection()
  {
    std::cout << "GammaCorrection::~GammaCorrection()\n";
  }

  float get_gamma();

  void set_gamma(float new_gamma);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

private:
  float gamma = 1.f;
};

class Primitive : public gnode::Node
{
public:
  Primitive(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap);

  // debugging
  ~Primitive()
  {
    std::cout << "Primitive::~Primitive() " << this->id.c_str() << "\n";
  }

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
