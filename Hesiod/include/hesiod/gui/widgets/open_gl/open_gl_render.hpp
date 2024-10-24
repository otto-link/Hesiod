/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file open_gl_render.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <filesystem>

#include <glm/glm.hpp>

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

#include "highmap/algebra.hpp"
#include "highmap/array.hpp"

#include "hesiod/gui/widgets/open_gl/open_gl_shaders.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

typedef unsigned int uint;

namespace hesiod
{

class OpenGLRender : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
  OpenGLRender(QWidget *parent = nullptr, ShaderType shader_type = ShaderType::TEXTURE);

  float get_h_scale() { return this->h_scale; }

  bool get_use_approx_mesh() { return this->use_approx_mesh; }

  bool get_wireframe_mode() { return this->wireframe_mode; }

  void on_node_compute_finished(const std::string &id);

  virtual void set_data(BaseNode          *new_p_node,
                        const std::string &new_port_id_elev,
                        const std::string &new_port_id_color);

  void set_h_scale(float new_h_scale)
  {
    this->h_scale = new_h_scale;
    this->repaint();
  }

  void set_max_approx_error(float new_max_approx_error);

  void set_shader_type(const ShaderType &new_shader_type);

  void set_use_approx_mesh(bool new_use_approx_mesh);

  void set_wireframe_mode(bool new_wireframe_mode);

  void setup_shader();

protected:
  void initializeGL() override;

  void mousePressEvent(QMouseEvent *event) override;

  void mouseMoveEvent(QMouseEvent *event) override;

  void paintGL() override;

  void resizeEvent(QResizeEvent *event) override;

  void resizeGL(int w, int h) override
  {
    this->glViewport(0, 0, w, h);
    this->repaint();
  }

  void set_data_again()
  {
    this->set_data(this->p_node, this->port_id_elev, this->port_id_color);
  }

  void wheelEvent(QWheelEvent *event) override;

protected:
  BaseNode   *p_node;
  std::string port_id_elev;
  std::string port_id_color;

  // OpenGL
  QOpenGLVertexArrayObject qvao;
  QOpenGLShaderProgram     shader;
  GLuint                   vbo;
  GLuint                   ebo;
  GLuint                   texture_id;
  GLuint                   normal_map_id;
  bool                     use_normal_map = true;
  ShaderType               shader_type;

  // mesh and texture
  std::vector<GLfloat> vertices = {};
  std::vector<uint>    indices = {};
  std::vector<uint8_t> texture_diffuse = {};
  std::vector<uint8_t> texture_normal_map = {};
  hmap::Vec2<int>      texture_shape = hmap::Vec2<int>(0, 0);

  bool  use_approx_mesh = true;
  float max_approx_error = 5e-3f;

  // view parameters
  float scale = 0.7f;
  float h_scale = 0.4f;
  float alpha_x = 35.f;
  float alpha_y = -25.f;
  float delta_x = 0.f;
  float delta_y = 0.f;

  float fov = 60.0f;
  float aspect_ratio = 1.f;
  float near_plane = 0.1f;
  float far_plane = 100.0f;

  bool wireframe_mode = false;

  QPointF mouse_pos_bckp;
  float   alpha_x_bckp, alpha_y_bckp, delta_x_bckp, delta_y_bckp;

  void bind_gl_buffers();
};

// helpers

std::vector<glm::vec3> compute_point_normals(const std::vector<glm::vec3>  &points,
                                             const std::vector<glm::ivec3> &triangles);

void generate_basemesh(hmap::Vec2<int>       shape,
                       std::vector<GLfloat> &vertices,
                       std::vector<uint>    &indices);

void generate_mesh_approximation(hmap::Array          &z,
                                 std::vector<GLfloat> &vertices,
                                 std::vector<uint>    &indices,
                                 float                 max_error,
                                 bool                  add_base = true,
                                 bool                  compute_normals = false);

void update_vertex_elevations(hmap::Array &z, std::vector<GLfloat> &vertices);

std::vector<uint8_t> generate_selector_image(hmap::Array &array);

} // namespace hesiod
