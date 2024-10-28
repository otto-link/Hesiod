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

  bool get_show_normal_map() { return this->show_normal_map; }

  bool get_wireframe_mode() { return this->wireframe_mode; }

  void on_node_compute_finished(const std::string &id);

  void set_azimuth(float new_azimuth);

  virtual void set_data(BaseNode          *new_p_node,
                        const std::string &new_port_id_elev,
                        const std::string &new_port_id_color,
                        const std::string &new_port_id_normal_map);

  void set_h_scale(float new_h_scale)
  {
    this->h_scale = new_h_scale;
    this->repaint();
  }

  void set_max_approx_error(float new_max_approx_error);

  void set_shader_type(const ShaderType &new_shader_type);

  void set_shadow_saturation(float new_shadow_saturation);

  void set_shadow_strength(float new_shadow_strength);

  void set_shadow_gamma(float new_shadow_gamma);

  void set_show_heightmap(bool new_show_heightmap);

  void set_show_normal_map(bool new_show_normal_map);

  void set_use_approx_mesh(bool new_use_approx_mesh);

  void set_wireframe_mode(bool new_wireframe_mode);

  void set_zenith(float new_zenith);

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
    this->set_data(this->p_node,
                   this->port_id_elev,
                   this->port_id_color,
                   this->port_id_normal_map);
  }

  void wheelEvent(QWheelEvent *event) override;

protected:
  BaseNode   *p_node;
  std::string port_id_elev;
  std::string port_id_color;
  std::string port_id_normal_map;

  // OpenGL
  QOpenGLVertexArrayObject qvao;
  QOpenGLShaderProgram     shader;
  GLuint                   vbo;
  GLuint                   ebo;
  GLuint                   texture_diffuse_id;
  GLuint                   texture_hmap_id;
  GLuint                   texture_normal_id;
  ShaderType               shader_type;

  // mesh and texture
  std::vector<GLfloat>  vertices = {};
  std::vector<uint>     indices = {};
  std::vector<uint8_t>  texture_diffuse = {};
  std::vector<uint16_t> texture_hmap = {};
  std::vector<uint8_t>  texture_normal = {};
  hmap::Vec2<int>       texture_diffuse_shape = hmap::Vec2<int>(0, 0);
  hmap::Vec2<int>       texture_hmap_shape = hmap::Vec2<int>(0, 0);
  hmap::Vec2<int>       texture_normal_shape = hmap::Vec2<int>(0, 0);

  // states whether the texture diffuse has been defined and is to be used in the shader
  bool use_texture_diffuse;
  bool use_texture_normal;

  // user view parameters
  float zmin = 0.f;
  float zmax = 1.f;

  float scale = 0.7f;
  float h_scale = 0.4f;
  float alpha_x = 35.f;
  float alpha_y = -25.f;
  float delta_x = 0.f;
  float delta_y = 0.f;

  float zenith = 0.f;
  float azimuth = 0.f;

  float shadow_saturation = 0.f;
  float shadow_strength = 1.f;
  float shadow_gamma = 1.f;

  float fov = 60.0f;
  float aspect_ratio = 1.f;
  float near_plane = 0.1f;
  float far_plane = 100.0f;

  bool wireframe_mode = false;
  bool show_normal_map = false;
  bool show_heightmap = false;

  bool  use_approx_mesh = false;
  float max_approx_error = 5e-3f;

  QPointF mouse_pos_bckp;
  float   alpha_x_bckp, alpha_y_bckp, delta_x_bckp, delta_y_bckp;

  void bind_gl_buffers();
};

// helpers

void generate_basemesh(hmap::Vec2<int>       shape,
                       std::vector<GLfloat> &vertices,
                       std::vector<uint>    &indices);

void generate_mesh_approximation(hmap::Array          &z,
                                 std::vector<GLfloat> &vertices,
                                 std::vector<uint>    &indices,
                                 float                 max_error);

std::vector<uint16_t> generate_grayscale_image_16bit(hmap::Array &array,
                                                     float        vmin = 0.f,
                                                     float        vmax = 1.f);

std::vector<uint8_t> generate_grayscale_image_8bit(hmap::Array &array,
                                                   float        vmin = 0.f,
                                                   float        vmax = 1.f);

std::vector<uint8_t> generate_selector_image(hmap::Array &array);

} // namespace hesiod
