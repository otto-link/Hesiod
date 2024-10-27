/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <glm/glm.hpp>

#include <QOpenGLFunctions>

#include "hmm/src/base.h" // from external/HighMap
#include "hmm/src/heightmap.h"
#include "hmm/src/triangulator.h"

#include "highmap/array.hpp"
#include "highmap/operator.hpp"

#include "hesiod/logger.hpp"

namespace hesiod
{

void generate_basemesh(hmap::Vec2<int>       shape,
                       std::vector<GLfloat> &vertices,
                       std::vector<uint>    &indices)
{
  LOG->trace("generate_basemesh, shape: ({}, {})", shape.x, shape.y);

  // vertices : (x, y, u, v)
  size_t n_vertices = shape.x * shape.y * 4;
  if (vertices.size() != n_vertices)
  {
    vertices.resize(n_vertices);

    // triangles vertex indices (2 triangles per quad and 3 indices per triangle)
    indices.resize((shape.x - 1) * (shape.y - 1) * 2 * 3);
  }

  bool               end_point = true;
  std::vector<float> x = hmap::linspace(-1.f, 1.f, shape.x, end_point);
  std::vector<float> y = hmap::linspace(1.f, -1.f, shape.y, end_point);

  int k = 0;
  for (int i = 0; i < shape.x; i++)
    for (int j = 0; j < shape.y; j++)
    {
      vertices[k++] = x[i];
      vertices[k++] = y[j];
      vertices[k++] = 0.5f * x[i] + 0.5f; // texture coordinates
      vertices[k++] = 0.5f * y[j] + 0.5f;
    }

  k = 0;
  for (int i = 0; i < shape.x - 1; i++)
    for (int j = 0; j < shape.y - 1; j++)
    {
      indices[k++] = i * shape.y + j;
      indices[k++] = (i + 1) * shape.y + j;
      indices[k++] = i * shape.y + j + 1;

      indices[k++] = (i + 1) * shape.y + j;
      indices[k++] = (i + 1) * shape.y + j + 1;
      indices[k++] = i * shape.y + j + 1;
    }
}

void generate_mesh_approximation(hmap::Array          &z,
                                 std::vector<GLfloat> &vertices,
                                 std::vector<uint>    &indices,
                                 float                 max_error)
{
  // NB x and y convention is permuted
  const auto   p_tri = std::make_shared<Heightmap>(z.shape.y, z.shape.x, z.get_vector());
  const int    max_triangles = 0;
  const int    max_points = 0;
  Triangulator tri(p_tri);

  LOG->trace("remeshing (Delaunay)");
  tri.Run(max_error, max_triangles, max_points);

  auto points = tri.Points(1.f);
  auto triangles = tri.Triangles();

  LOG->trace("points: {} (max: {})", points.size(), z.size());
  LOG->trace("triangles: {}", triangles.size());

  // send mesh to GL buffer
  vertices.clear();
  indices.clear();
  indices.reserve(3 * triangles.size());

  float ax = 1.f / (z.shape.x - 1.f);
  float ay = 1.f / (z.shape.y - 1.f);

  vertices.reserve(4 * points.size());

  for (auto &p : points)
  {
    // position
    vertices.push_back(1.f - 2.f * ay * p.y);
    vertices.push_back(1.f - 2.f * ax * p.x);

    // uv
    vertices.push_back(1.f - p.y * ay);
    vertices.push_back(1.f - p.x * ax);
  }

  for (auto &t : triangles)
  {
    indices.push_back(t.x);
    indices.push_back(t.y);
    indices.push_back(t.z);
  }
}

std::vector<uint16_t> generate_grayscale_image_16bit(hmap::Array &array,
                                                     float        vmin,
                                                     float        vmax)
{
  std::vector<uint16_t> img(array.shape.x * array.shape.y);

  float coeff = vmin == vmax ? 0.f : 1.f / (vmax - vmin);

  int k = 0;
  for (int j = array.shape.y - 1; j > -1; j--)
    for (int i = 0; i < array.shape.x; i++)
    {
      float v = (array(i, j) - vmin) * coeff;
      img[k++] = (uint16_t)(65535.f * v);
    }

  return img;
}

std::vector<uint8_t> generate_grayscale_image_8bit(hmap::Array &array,
                                                   float        vmin,
                                                   float        vmax)
{
  std::vector<uint8_t> img(array.shape.x * array.shape.y);

  float coeff = vmin == vmax ? 0.f : 1.f / (vmax - vmin);

  int k = 0;
  for (int j = array.shape.y - 1; j > -1; j--)
    for (int i = 0; i < array.shape.x; i++)
    {
      float v = (array(i, j) - vmin) * coeff;
      img[k++] = (uint8_t)(255.f * v);
    }

  return img;
}

std::vector<uint8_t> generate_selector_image(hmap::Array &array)
{
  std::vector<uint8_t> img(4 * array.shape.x * array.shape.y);

  int k = 0;
  for (int j = array.shape.y - 1; j > -1; j--)
    for (int i = 0; i < array.shape.x; i++)
    {
      img[k++] = (uint8_t)(255.f);
      img[k++] = (uint8_t)(255.f * (1.f - array(i, j)));
      img[k++] = (uint8_t)(255.f);
      img[k++] = (uint8_t)(255.f);
    }

  return img;
}

} // namespace hesiod
