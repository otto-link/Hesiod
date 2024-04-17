/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QMouseEvent>

#include "macrologger.h"

#include "glm/gtc/matrix_transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "highmap/heightmap.hpp"
#include "highmap/op.hpp"

#include "hesiod/data/cloud_data.hpp"
#include "hesiod/data/heightmap_data.hpp"
#include "hesiod/data/heightmap_rgba_data.hpp"
#include "hesiod/data/mask_data.hpp"
#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

// --- helpers

void generate_basemesh(hmap::Vec2<int>       shape,
                       std::vector<GLfloat> &vertices,
                       std::vector<GLfloat> &colors)
{
  size_t n_vertices = (shape.x - 1) * (shape.y - 1) * 3 * 3 * 2;
  if (vertices.size() != n_vertices)
  {
    vertices.resize(n_vertices);

    size_t n_colors = (shape.x - 1) * (shape.y - 1) * 3 * 4 * 2;
    colors.resize(n_colors);
  }

  std::vector<float> x = hmap::linspace(-1.f, 1.f, shape.x);
  std::vector<float> y = hmap::linspace(1.f, -1.f, shape.y);

  int k = 0;

  for (int i = 0; i < shape.x - 1; i++)
    for (int j = 0; j < shape.y - 1; j++)
    {
      vertices[k++] = x[i];
      vertices[k++] = 0.f;
      vertices[k++] = y[j];

      vertices[k++] = x[i];
      vertices[k++] = 0.f;
      vertices[k++] = y[j + 1];

      vertices[k++] = x[i + 1];
      vertices[k++] = 0.f;
      vertices[k++] = y[j + 1];

      //
      vertices[k++] = x[i];
      vertices[k++] = 0.f;
      vertices[k++] = y[j];

      vertices[k++] = x[i + 1];
      vertices[k++] = 0.f;
      vertices[k++] = y[j + 1];

      vertices[k++] = x[i + 1];
      vertices[k++] = 0.f;
      vertices[k++] = y[j];
    }
}

void update_vertex_elevations(hmap::Array &z, std::vector<GLfloat> &vertices)
{
  int k = 1;

  for (int i = 0; i < z.shape.x - 1; i++)
    for (int j = 0; j < z.shape.y - 1; j++)
    {
      vertices[k] = z(i, j);
      k += 3;

      vertices[k] = z(i, j + 1);
      k += 3;

      vertices[k] = z(i + 1, j + 1);
      k += 3;

      //
      vertices[k] = z(i, j);
      k += 3;

      vertices[k] = z(i + 1, j + 1);
      k += 3;

      vertices[k] = z(i + 1, j);
      k += 3;
    }
}

void update_vertex_colors(hmap::Array          &z,
                          std::vector<GLfloat> &colors,
                          hmap::Array          *p_alpha)
{
  hmap::Array hs = hillshade(z, 180.f, 45.f, 10.f * z.ptp() / (float)z.shape.y);
  hs = hmap::pow(hs, 1.5f);

  int k = 0;

  if (p_alpha)
    for (int i = 0; i < z.shape.x - 1; i++)
      for (int j = 0; j < z.shape.y - 1; j++)
      {
        colors[k++] = hs(i, j);
        colors[k++] = hs(i, j);
        colors[k++] = hs(i, j);
        colors[k++] = (*p_alpha)(i, j);

        colors[k++] = hs(i, j + 1);
        colors[k++] = hs(i, j + 1);
        colors[k++] = hs(i, j + 1);
        colors[k++] = (*p_alpha)(i, j + 1);

        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = (*p_alpha)(i + 1, j + 1);

        colors[k++] = hs(i, j);
        colors[k++] = hs(i, j);
        colors[k++] = hs(i, j);
        colors[k++] = (*p_alpha)(i, j);

        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = (*p_alpha)(i + 1, j + 1);

        colors[k++] = hs(i + 1, j);
        colors[k++] = hs(i + 1, j);
        colors[k++] = hs(i + 1, j);
        colors[k++] = (*p_alpha)(i + 1, j);
      }
  else
    for (int i = 0; i < z.shape.x - 1; i++)
      for (int j = 0; j < z.shape.y - 1; j++)
      {
        colors[k++] = hs(i, j);
        colors[k++] = hs(i, j);
        colors[k++] = hs(i, j);
        colors[k++] = 1.f;

        colors[k++] = hs(i, j + 1);
        colors[k++] = hs(i, j + 1);
        colors[k++] = hs(i, j + 1);
        colors[k++] = 1.f;

        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = 1.f;

        colors[k++] = hs(i, j);
        colors[k++] = hs(i, j);
        colors[k++] = hs(i, j);
        colors[k++] = 1.f;

        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = 1.f;

        colors[k++] = hs(i + 1, j);
        colors[k++] = hs(i + 1, j);
        colors[k++] = hs(i + 1, j);
        colors[k++] = 1.f;
      }
}

void update_vertex_colors(hmap::Array          &z,
                          hmap::Array          &c,
                          std::vector<GLfloat> &colors,
                          hmap::Array          *p_alpha)
{
  hmap::Array hs = hillshade(z, 180.f, 45.f, 10.f * z.ptp() / (float)z.shape.y);
  hs = hmap::pow(hs, 1.5f);

  int k = 0;

  if (p_alpha)
    for (int i = 0; i < z.shape.x - 1; i++)
      for (int j = 0; j < z.shape.y - 1; j++)
      {
        colors[k++] = hs(i, j);
        colors[k++] = hs(i, j) * c(i, j);
        colors[k++] = hs(i, j);
        colors[k++] = (*p_alpha)(i, j);

        colors[k++] = hs(i, j + 1);
        colors[k++] = hs(i, j + 1) * c(i, j + 1);
        colors[k++] = hs(i, j + 1);
        colors[k++] = (*p_alpha)(i, j + 1);

        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1) * c(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = (*p_alpha)(i + 1, j + 1);

        colors[k++] = hs(i, j);
        colors[k++] = hs(i, j) * c(i, j);
        colors[k++] = hs(i, j);
        colors[k++] = (*p_alpha)(i, j);

        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1) * c(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = (*p_alpha)(i + 1, j + 1);

        colors[k++] = hs(i + 1, j);
        colors[k++] = hs(i + 1, j) * c(i + 1, j);
        colors[k++] = hs(i + 1, j);
        colors[k++] = (*p_alpha)(i + 1, j);
      }
  else
    for (int i = 0; i < z.shape.x - 1; i++)
      for (int j = 0; j < z.shape.y - 1; j++)
      {
        colors[k++] = hs(i, j);
        colors[k++] = hs(i, j) * c(i, j);
        colors[k++] = hs(i, j);
        colors[k++] = 1.f;

        colors[k++] = hs(i, j + 1);
        colors[k++] = hs(i, j + 1) * c(i, j + 1);
        colors[k++] = hs(i, j + 1);
        colors[k++] = 1.f;

        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1) * c(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = 1.f;

        colors[k++] = hs(i, j);
        colors[k++] = hs(i, j) * c(i, j);
        colors[k++] = hs(i, j);
        colors[k++] = 1.f;

        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1) * c(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1);
        colors[k++] = 1.f;

        colors[k++] = hs(i + 1, j);
        colors[k++] = hs(i + 1, j) * c(i + 1, j);
        colors[k++] = hs(i + 1, j);
        colors[k++] = 1.f;
      }
}

void update_vertex_colors(hmap::Array          &z,
                          hmap::Array          &r,
                          hmap::Array          &g,
                          hmap::Array          &b,
                          std::vector<GLfloat> &colors,
                          hmap::Array          *p_alpha)
{
  hmap::Array hs = hillshade(z, 180.f, 45.f, 10.f * z.ptp() / (float)z.shape.y);
  hs = hmap::pow(hs, 0.9f);
  hmap::remap(hs, 0.f, 1.5f);

  int k = 0;

  if (p_alpha)
    for (int i = 0; i < z.shape.x - 1; i++)
      for (int j = 0; j < z.shape.y - 1; j++)
      {
        colors[k++] = hs(i, j) * r(i, j);
        colors[k++] = hs(i, j) * g(i, j);
        colors[k++] = hs(i, j) * b(i, j);
        colors[k++] = (*p_alpha)(i, j);

        colors[k++] = hs(i, j + 1) * r(i, j + 1);
        colors[k++] = hs(i, j + 1) * g(i, j + 1);
        colors[k++] = hs(i, j + 1) * b(i, j + 1);
        colors[k++] = (*p_alpha)(i, j + 1);

        colors[k++] = hs(i + 1, j + 1) * r(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1) * g(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1) * b(i + 1, j + 1);
        colors[k++] = (*p_alpha)(i + 1, j + 1);

        colors[k++] = hs(i, j) * r(i, j);
        colors[k++] = hs(i, j) * g(i, j);
        colors[k++] = hs(i, j) * b(i, j);
        colors[k++] = (*p_alpha)(i, j);

        colors[k++] = hs(i + 1, j + 1) * r(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1) * g(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1) * b(i + 1, j + 1);
        colors[k++] = (*p_alpha)(i + 1, j + 1);

        colors[k++] = hs(i + 1, j) * r(i + 1, j);
        colors[k++] = hs(i + 1, j) * g(i + 1, j);
        colors[k++] = hs(i + 1, j) * b(i + 1, j);
        colors[k++] = (*p_alpha)(i + 1, j);
      }
  else
    for (int i = 0; i < z.shape.x - 1; i++)
      for (int j = 0; j < z.shape.y - 1; j++)
      {
        colors[k++] = hs(i, j) * r(i, j);
        colors[k++] = hs(i, j) * g(i, j);
        colors[k++] = hs(i, j) * b(i, j);
        colors[k++] = 1.f;

        colors[k++] = hs(i, j + 1) * r(i, j + 1);
        colors[k++] = hs(i, j + 1) * g(i, j + 1);
        colors[k++] = hs(i, j + 1) * b(i, j + 1);
        colors[k++] = 1.f;

        colors[k++] = hs(i + 1, j + 1) * r(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1) * g(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1) * b(i + 1, j + 1);
        colors[k++] = 1.f;

        colors[k++] = hs(i, j) * r(i, j);
        colors[k++] = hs(i, j) * g(i, j);
        colors[k++] = hs(i, j) * b(i, j);
        colors[k++] = 1.f;

        colors[k++] = hs(i + 1, j + 1) * r(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1) * g(i + 1, j + 1);
        colors[k++] = hs(i + 1, j + 1) * b(i + 1, j + 1);
        colors[k++] = 1.f;

        colors[k++] = hs(i + 1, j) * r(i + 1, j);
        colors[k++] = hs(i + 1, j) * g(i + 1, j);
        colors[k++] = hs(i + 1, j) * b(i + 1, j);
        colors[k++] = 1.f;
      }
}

// --- class definitions

HmapGLViewer::HmapGLViewer(ModelConfig       *p_config,
                           QtNodes::NodeData *p_data,
                           QtNodes::NodeData *p_color,
                           QWidget           *parent)
    : QOpenGLWidget(parent), p_config(p_config), p_data(p_data), p_color(p_color)
{
  // generate_basemesh(p_config->shape, this->vertices, this->colors);
}

void HmapGLViewer::bind_gl_buffers()
{
  // make sure OpenGL context is set
  this->makeCurrent(); // QOpenGLWidget

  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(this->vertices[0]) * this->vertices.size(),
               (GLvoid *)&this->vertices[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_colors);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(this->colors[0]) * this->colors.size(),
               (GLvoid *)&this->colors[0],
               GL_STATIC_DRAW);
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

    if (data_type.compare("HeightMapData") == 0 || data_type.compare("MaskData") == 0)
    {
      // elevation array
      hmap::HeightMap *p_h;
      hmap::Array      array;

      if (data_type.compare("HeightMapData") == 0)
      {
        HeightMapData *p_hdata = static_cast<HeightMapData *>(this->p_data);
        p_h = static_cast<hmap::HeightMap *>(p_hdata->get_ref());
        array = p_h->to_array();
      }
      else if (data_type.compare("MaskData") == 0)
      {
        MaskData *p_hdata = static_cast<MaskData *>(this->p_data);
        p_h = static_cast<hmap::HeightMap *>(p_hdata->get_ref());
        array = p_h->to_array();
      }

      // generate the basemesh (NB - shape can be modified while
      // editing the graph when the model configuration is changed by
      // the user)
      if (array.size() != this->vertices.size())
        generate_basemesh(array.shape, this->vertices, this->colors);

      update_vertex_elevations(array, this->vertices);

      // color array
      if (p_color)
      {
        QString color_type = this->p_color->type().id;

        if (color_type.compare("HeightMapData") == 0)
        {
          HeightMapData   *p_hcolor = static_cast<HeightMapData *>(this->p_color);
          hmap::HeightMap *p_c = static_cast<hmap::HeightMap *>(p_hcolor->get_ref());
          hmap::Array      c = 1.f - p_c->to_array();

          update_vertex_colors(array, c, this->colors, nullptr);
        }
        else if (color_type.compare("HeightMapRGBAData") == 0)
        {
          HeightMapRGBAData   *p_hcolor = static_cast<HeightMapRGBAData *>(this->p_color);
          hmap::HeightMapRGBA *p_c = static_cast<hmap::HeightMapRGBA *>(
              p_hcolor->get_ref());

          hmap::Array r = p_c->rgba[0].to_array();
          hmap::Array g = p_c->rgba[1].to_array();
          hmap::Array b = p_c->rgba[2].to_array();
          hmap::Array a = p_c->rgba[3].to_array();

          update_vertex_colors(array, r, g, b, this->colors, &a);
        }
        else if (color_type.compare("MaskData") == 0)
        {
          MaskData        *p_hcolor = static_cast<MaskData *>(this->p_color);
          hmap::HeightMap *p_c = static_cast<hmap::HeightMap *>(p_hcolor->get_ref());
          hmap::Array      c = 1.f - p_c->to_array();

          update_vertex_colors(array, c, this->colors, nullptr);
        }
        else if (color_type.compare("CloudData") == 0)
        {
          CloudData   *p_hcolor = static_cast<CloudData *>(this->p_color);
          hmap::Cloud *p_cloud = static_cast<hmap::Cloud *>(p_hcolor->get_ref());
          hmap::Array  c = hmap::Array(array.shape);
          p_cloud->to_array(c);
          c *= -1.f;
          c += 1.f;

          update_vertex_colors(array, c, this->colors, nullptr);
        }
      }
      else
        update_vertex_colors(array, this->colors, nullptr);

      // send to OpenGL buffers
      this->bind_gl_buffers();
    }
  }

  this->repaint();
}

void HmapGLViewer::initializeGL()
{
  this->initializeOpenGLFunctions(); // QOpenGLFunctions
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glGenBuffers(1, &this->vbo_vertices);
  glGenBuffers(1, &this->vbo_colors);

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

  // Compile shaders
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source_pointer, NULL);
  glCompileShader(vertex_shader);
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source_pointer, NULL);
  glCompileShader(fragment_shader);

  // Link shaders
  this->shader_program = glCreateProgram();
  glAttachShader(this->shader_program, vertex_shader);
  glAttachShader(this->shader_program, fragment_shader);
  glLinkProgram(this->shader_program);

  // Delete shaders
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
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
    glUseProgram(this->shader_program);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_colors);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);

    // transformations
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
    GLuint model_matrix_location = glGetUniformLocation(shader_program, "modelMatrix");
    glUniformMatrix4fv(model_matrix_location,
                       1,
                       GL_FALSE,
                       glm::value_ptr(combined_matrix));

    // eventually render
    glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
  }
}

void HmapGLViewer::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

} // namespace hesiod
