/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QOpenGLFunctions>

#include "highmap/array.hpp"
#include "highmap/operator.hpp"

namespace hesiod
{

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
