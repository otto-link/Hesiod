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

std::vector<glm::vec3> compute_point_normals(const std::vector<glm::vec3>  &points,
                                             const std::vector<glm::ivec3> &triangles)
{
  // create a container for storing normals per point
  std::vector<glm::vec3> point_normals(points.size(), glm::vec3(0.0f));

  // iterate through each triangle to compute normals
  for (const auto &triangle : triangles)
  {
    // get the vertices of the triangle
    glm::vec3 p0 = points[triangle.x];
    glm::vec3 p1 = points[triangle.y];
    glm::vec3 p2 = points[triangle.z];

    // compute two edge vectors
    glm::vec3 edge1 = p1 - p0;
    glm::vec3 edge2 = p2 - p0;

    // compute the cross product (normal to the triangle surface)
    glm::vec3 triangle_normal = glm::normalize(glm::cross(edge1, edge2));

    // accumulate the normal for each vertex of the triangle
    point_normals[triangle.x] += triangle_normal;
    point_normals[triangle.y] += triangle_normal;
    point_normals[triangle.z] += triangle_normal;
  }

  // Normalize the accumulated normals at each point
  for (auto &normal : point_normals)
    normal = glm::normalize(normal);

  return point_normals;
}

void generate_basemesh(hmap::Vec2<int>       shape,
                       std::vector<GLfloat> &vertices,
                       std::vector<uint>    &indices)
{
  // vertices : (x, elevation, y, u, v)
  size_t n_vertices = shape.x * shape.y * 5;
  if (vertices.size() != n_vertices)
  {
    vertices.resize(n_vertices);

    // triangles vertex indices (2 triangles per quad and 3 indices per triangle)
    indices.resize((shape.x - 1) * (shape.y - 1) * 2 * 3);
  }

  std::vector<float> x = hmap::linspace(-1.f, 1.f, shape.x);
  std::vector<float> y = hmap::linspace(1.f, -1.f, shape.y);

  int k = 0;
  for (int i = 0; i < shape.x; i++)
    for (int j = 0; j < shape.y; j++)
    {
      vertices[k++] = x[i];
      vertices[k++] = 0.f;
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
                                 float                 max_error,
                                 bool                  add_base,
                                 bool                  compute_normals)
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

  if (add_base)
    AddBase(points, triangles, z.shape.y, z.shape.x, z.min());

  LOG->trace("points: {} (max: {})", points.size(), z.size());
  LOG->trace("triangles: {}", triangles.size());

  // normals
  std::vector<glm::vec3> normals = {};
  if (compute_normals)
    normals = compute_point_normals(points, triangles);

  // send mesh to GL buffer
  vertices.clear();
  indices.clear();
  indices.reserve(3 * triangles.size());

  float ax = 1.f / (z.shape.x - 1.f);
  float ay = 1.f / (z.shape.y - 1.f);

  if (compute_normals)
  {
    vertices.reserve(8 * points.size());

    for (size_t k = 0; k < points.size(); k++)
    {
      glm::vec3 p = points[k];

      // position
      vertices.push_back(2.f * ay * p.y - 1.f);
      vertices.push_back(p.z);
      vertices.push_back(2.f * ax * p.x - 1.f);

      // uv
      vertices.push_back(1.f - p.y * ay);
      vertices.push_back(1.f - p.x * ax);

      // normals
      vertices.push_back(z.shape.x * normals[k].x);
      vertices.push_back(z.shape.y * normals[k].y);
      vertices.push_back(normals[k].z);
    }
  }
  else
  {
    vertices.reserve(5 * points.size());

    for (auto &p : points)
    {
      // position
      vertices.push_back(2.f * ay * p.y - 1.f);
      vertices.push_back(p.z);
      vertices.push_back(2.f * ax * p.x - 1.f);

      // uv
      vertices.push_back(1.f - p.y * ay);
      vertices.push_back(1.f - p.x * ax);
    }
  }

  for (auto &t : triangles)
  {
    indices.push_back(t.x);
    indices.push_back(t.y);
    indices.push_back(t.z);
  }
}

void update_vertex_elevations(hmap::Array &z, std::vector<GLfloat> &vertices)
{
  int k = 1;

  for (int i = 0; i < z.shape.x; i++)
    for (int j = 0; j < z.shape.y; j++)
    {
      vertices[k] = z(i, j);
      k += 5;
    }
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
