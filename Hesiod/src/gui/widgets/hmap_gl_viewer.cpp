/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QMouseEvent>

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>

#include <QDebug>

#include "macrologger.h"

#include "glm/gtc/matrix_transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "highmap/heightmap.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"

#include "hesiod/data/cloud_data.hpp"
#include "hesiod/data/heightmap_data.hpp"
#include "hesiod/data/heightmap_rgba_data.hpp"
#include "hesiod/data/path_data.hpp"
#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

// --- helpers

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

  hmap::Array hs = hillshade(array,
                             180.f,
                             45.f,
                             10.f * array.ptp() / (float)array.shape.x);
  hmap::remap(hs);
  hs = hmap::pow(hs, 1.5f);

  int k = 0;
  for (int j = array.shape.y - 1; j > -1; j--)
    for (int i = 0; i < array.shape.x; i++)
    {
      img[k++] = (uint8_t)(255.f * hs(i, j));
      img[k++] = (uint8_t)(255.f * (1.f - array(i, j)) * hs(i, j));
      img[k++] = (uint8_t)(255.f * hs(i, j));
      img[k++] = (uint8_t)(255.f);
    }

  return img;
}

// --- class definitions

HmapGLViewer::HmapGLViewer(ModelConfig       *p_config,
                           QtNodes::NodeData *p_data,
                           QtNodes::NodeData *p_color,
                           QWidget           *parent)
    : QOpenGLWidget(parent), p_config(p_config), p_data(p_data), p_color(p_color)
{
}

void HmapGLViewer::bind_gl_buffers()
{
  // make sure OpenGL context is set
  this->makeCurrent(); // QOpenGLWidget

  this->qvao.bind();

  // vertices
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(this->vertices[0]) * this->vertices.size(),
               (GLvoid *)&this->vertices[0],
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // face indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(this->indices[0]) * this->indices.size(),
               (GLvoid *)&this->indices[0],
               GL_STATIC_DRAW);

  // texture
  glBindTexture(GL_TEXTURE_2D, this->texture_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               texture_shape.x,
               texture_shape.y,
               0,
               GL_RGBA,
               GL_UNSIGNED_BYTE,
               this->texture_img.data());
  glGenerateMipmap(GL_TEXTURE_2D);

  // we're done...
  this->qvao.release();
}

void HmapGLViewer::reset() { this->set_data(nullptr, nullptr); }

void HmapGLViewer::set_data(QtNodes::NodeData *new_p_data, QtNodes::NodeData *new_p_color)
{
  this->p_data = new_p_data;
  this->p_color = new_p_color;

  if (p_data)
  {

    // --- prepare data

    QString data_type = this->p_data->type().id;

    if (data_type.compare("HeightMapData") == 0)
    {
      // elevation array
      hmap::HeightMap *p_h;
      hmap::Array      array;

      HeightMapData *p_hdata = static_cast<HeightMapData *>(this->p_data);
      p_h = static_cast<hmap::HeightMap *>(p_hdata->get_ref());
      array = p_h->to_array();

      // generate the basemesh (NB - shape can be modified while
      // editing the graph when the model configuration is changed by
      // the user)
      if (array.size() != (int)this->vertices.size())
        generate_basemesh(array.shape, this->vertices, this->indices);

      update_vertex_elevations(array, this->vertices);

      // color array
      bool color_done = false;

      if (p_color)
      {
        QString color_type = this->p_color->type().id;

        if (color_type.compare("HeightMapData") == 0 && this->render_mask)
        {
          HeightMapData   *p_hcolor = static_cast<HeightMapData *>(this->p_color);
          hmap::HeightMap *p_c = static_cast<hmap::HeightMap *>(p_hcolor->get_ref());
          hmap::Array      c = 1.f - p_c->to_array();

          this->texture_img = generate_selector_image(array);
          this->texture_shape = array.shape;
          color_done = true;
        }
        //
        else if (color_type.compare("HeightMapRGBAData") == 0 && this->render_texture)
        {
          HeightMapRGBAData   *p_hcolor = static_cast<HeightMapRGBAData *>(this->p_color);
          hmap::HeightMapRGBA *p_c = static_cast<hmap::HeightMapRGBA *>(
              p_hcolor->get_ref());

          this->texture_img = p_c->to_img_8bit(p_c->shape);
          this->texture_shape = p_c->shape;

          // 'true' for RGBA, '0.9f' is exponent applied to shadows
          hmap::apply_hillshade(this->texture_img, array, 0.f, 1.5f, 0.9f, true);

          color_done = true;
        }
        //
        else if (color_type.compare("PathData") == 0)
        {
          PathData   *p_hcolor = static_cast<PathData *>(this->p_color);
          hmap::Path *p_path = static_cast<hmap::Path *>(p_hcolor->get_ref());
          hmap::Array c = hmap::Array(array.shape);

          hmap::Path path_copy = *p_path;
          path_copy.set_values(1.f);
          hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
          path_copy.to_array(c, bbox);
          // c = 1.f - c;

          this->texture_img = generate_selector_image(c);
          this->texture_shape = array.shape;

          hmap::apply_hillshade(this->texture_img, array, 0.f, 1.f, 1.5f, true);

          color_done = true;
        }
      }

      // default rendering
      if (!color_done)
      {
        this->texture_img.resize(4 * array.shape.x * array.shape.y);

        hmap::Array hs = hillshade(array,
                                   180.f,
                                   45.f,
                                   10.f * array.ptp() / (float)array.shape.x);
        hmap::remap(hs);
        hs = hmap::pow(hs, 1.5f);

        int k = 0;
        for (int j = array.shape.y - 1; j > -1; j--)
          for (int i = 0; i < array.shape.x; i++)
          {
            this->texture_img[k++] = (uint8_t)(255.f * hs(i, j));
            this->texture_img[k++] = (uint8_t)(255.f * hs(i, j));
            this->texture_img[k++] = (uint8_t)(255.f * hs(i, j));
            this->texture_img[k++] = (uint8_t)(255.f);
          }

        this->texture_shape = array.shape;
      }

      // send to OpenGL buffers
      this->bind_gl_buffers();
    }
  }

  this->repaint();
}

void HmapGLViewer::set_data_forced() { this->set_data(this->p_data, this->p_color); }

void HmapGLViewer::initializeGL()
{
  this->initializeOpenGLFunctions(); // QOpenGLFunctions
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glGenBuffers(1, &this->vbo);
  glGenBuffers(1, &this->ebo);
  glGenTextures(1, &this->texture_id);

  // read shader sources
  std::string vertex_shader_code;
  {
    const std::string code =
#include "vertex_shader.vs"
        ;
    vertex_shader_code = code.c_str();
  }
  char const *vertex_shader_source_pointer = vertex_shader_code.c_str();

  std::string fragment_shader_code;
  {
    const std::string code =
#include "fragment_shader.vs"
        ;
    fragment_shader_code = code.c_str();
  }
  char const *fragment_shader_source_pointer = fragment_shader_code.c_str();

  // compile shaders
  if (!this->shader.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                            vertex_shader_source_pointer))
    throw std::runtime_error("Failed to compile vertex shader");

  if (!this->shader.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                            fragment_shader_source_pointer))
    throw std::runtime_error("Failed to compile fragment shader");

  // bind
  this->shader.bind();

  this->qvao.create();
  this->qvao.bind();
  this->qvao.release();

  this->shader.release();
}

void HmapGLViewer::mouseMoveEvent(QMouseEvent *event)
{
  QPointF delta = event->pos() - this->mouse_pos_bckp;

  if (event->buttons() == Qt::LeftButton)
  {
    float scaling = 0.1f;

    this->alpha_y = this->alpha_y_bckp + scaling * delta.x();
    this->alpha_x = this->alpha_x_bckp + scaling * delta.y();
  }

  if (event->buttons() == Qt::RightButton)
  {
    float scaling = 0.002f;

    this->delta_x = this->delta_x_bckp + scaling * delta.x();
    this->delta_y = this->delta_y_bckp - scaling * delta.y();
  }

  if (event->buttons() == Qt::LeftButton || event->buttons() == Qt::RightButton)
    this->repaint();
}

void HmapGLViewer::mousePressEvent(QMouseEvent *event)
{
  if (event->buttons() == Qt::LeftButton)
  {
    this->mouse_pos_bckp = event->pos();
    this->alpha_x_bckp = this->alpha_x;
    this->alpha_y_bckp = this->alpha_y;
  }

  if (event->buttons() == Qt::RightButton)
  {
    this->mouse_pos_bckp = event->pos();
    this->delta_x_bckp = this->delta_x;
    this->delta_y_bckp = this->delta_y;
  }
}

void HmapGLViewer::wheelEvent(QWheelEvent *event)
{
  float dscale = this->scale * 0.1f;

  if (event->delta() > 0)
    this->scale = std::max(0.05f, this->scale + dscale);
  else
    this->scale = std::max(0.05f, this->scale - dscale);

  this->repaint();
}

void HmapGLViewer::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (this->p_data)
  {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    this->shader.bind();

    // --- transformations

    glm::mat4 combined_matrix;
    {
      // compensate for viewport aspect ratio
      this->aspect_ratio = (float)this->width() / (float)this->height();

      glm::mat4 scale_matrix = glm::scale(
          glm::mat4(1.0f),
          glm::vec3(this->scale, this->scale * this->h_scale, this->scale));

      glm::mat4 rotation_matrix_y = glm::rotate(glm::mat4(1.0f),
                                                glm::radians(this->alpha_y),
                                                glm::vec3(0.0f, 1.0f, 0.0f));

      glm::mat4 rotation_matrix_x = glm::rotate(glm::mat4(1.0f),
                                                glm::radians(this->alpha_x),
                                                glm::vec3(1.0f, 0.0f, 0.0f));

      glm::mat4 rotation_matrix = rotation_matrix_x * rotation_matrix_y;

      glm::mat4 transalation_matrix = glm::translate(
          glm::mat4(1.f),
          glm::vec3(this->delta_x, this->delta_y, 0.f));

      glm::mat4 view_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -2.f));

      // define perspective projection parameters
      glm::mat4 projection_matrix = glm::perspective(glm::radians(this->fov),
                                                     this->aspect_ratio,
                                                     this->near_plane,
                                                     this->far_plane);

      combined_matrix = projection_matrix * view_matrix * transalation_matrix *
                        rotation_matrix * scale_matrix;
    }

    this->shader.setUniformValue(
        "modelMatrix",
        QMatrix4x4(glm::value_ptr(combined_matrix)).transposed());

    // --- eventually render the surface

    this->qvao.bind();
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    this->qvao.release();
  }
}

void HmapGLViewer::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

} // namespace hesiod
