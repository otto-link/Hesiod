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

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

#include "highmap/algebra.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

class OpenGLRender : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
  OpenGLRender(QWidget *parent = nullptr);

  void set_data(BaseNode          *new_p_node,
                const std::string &new_port_id_elev,
                const std::string &new_port_id_color);

protected:
  void initializeGL() override;

  void mousePressEvent(QMouseEvent *event) override;

  void mouseMoveEvent(QMouseEvent *event) override;

  void paintGL() override;

  void resizeEvent(QResizeEvent *event) override;

  void resizeGL(int w, int h) override { this->glViewport(0, 0, w, h); }

  void wheelEvent(QWheelEvent *event) override;

private:
  BaseNode   *p_node;
  std::string port_id_elev;
  std::string port_id_color;

  // OpenGL
  QOpenGLVertexArrayObject qvao;
  QOpenGLShaderProgram     shader;
  GLuint                   vbo;
  GLuint                   ebo;
  GLuint                   texture_id;

  // mesh and texture
  std::vector<GLfloat> vertices = {};
  std::vector<uint>    indices = {};
  std::vector<uint8_t> texture_img = {};
  hmap::Vec2<int>      texture_shape = hmap::Vec2<int>(0, 0);

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

  QPointF mouse_pos_bckp;
  float   alpha_x_bckp, alpha_y_bckp, delta_x_bckp, delta_y_bckp;

  void bind_gl_buffers();

  void set_shader();
};

// helpers

void generate_basemesh(hmap::Vec2<int>       shape,
                       std::vector<GLfloat> &vertices,
                       std::vector<uint>    &indices);

void update_vertex_elevations(hmap::Array &z, std::vector<GLfloat> &vertices);

std::vector<uint8_t> generate_selector_image(hmap::Array &array);

} // namespace hesiod