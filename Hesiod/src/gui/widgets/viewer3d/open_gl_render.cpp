/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "glm/gtc/matrix_transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "highmap/colorize.hpp"
#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/heightmap.hpp"
#include "highmap/operator.hpp"
#include "highmap/range.hpp"
#include "highmap/shadows.hpp"

#include "hesiod/gui/widgets/open_gl/open_gl_render.hpp"
#include "hesiod/gui/widgets/open_gl/open_gl_shaders.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

OpenGLRender::OpenGLRender(QWidget *parent, ShaderType shader_type)
    : QOpenGLWidget(parent), p_node(nullptr), port_id_elev(""), port_id_color(""),
      shader_type(shader_type)
{
  LOG->trace("OpenGLRender::OpenGLRender");

  this->setMinimumSize(256, 256);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void OpenGLRender::bind_gl_buffers()
{
  // make sure OpenGL context is set
  QOpenGLWidget::makeCurrent();

  this->shader.bind();
  this->qvao.bind();

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  // vertices
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(this->vertices[0]) * this->vertices.size(),
               (GLvoid *)&this->vertices[0],
               GL_STATIC_DRAW);

  // int nfields = this->use_normals ? 8 : 5;
  int nfields = 5;

  // postion
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, nfields * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // texture uv coordinates
  glVertexAttribPointer(1,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        nfields * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // // normals (if any)
  // if (this->use_normals)
  // {
  //   glVertexAttribPointer(2,
  //                         3,
  //                         GL_FLOAT,
  //                         GL_FALSE,
  //                         nfields * sizeof(float),
  //                         (void *)(5 * sizeof(float)));
  //   glEnableVertexAttribArray(2);
  // }

  // face indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(this->indices[0]) * this->indices.size(),
               (GLvoid *)&this->indices[0],
               GL_STATIC_DRAW);

  // texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, this->texture_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               this->texture_shape.x,
               this->texture_shape.y,
               0,
               GL_RGBA,
               GL_UNSIGNED_BYTE,
               this->texture_diffuse.data());
  glGenerateMipmap(GL_TEXTURE_2D);

  // assign to texture sampler #0
  this->shader.setUniformValue("textureDiffuse", 0);

  // normal map
  if (this->use_normal_map)
  {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->normal_map_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 this->texture_shape.x,
                 this->texture_shape.y,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 this->texture_normal_map.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    // assign to texture sampler #1
    this->shader.setUniformValue("normalMap", 1);
  }

  // and... we're done
  this->qvao.release();
}

void OpenGLRender::initializeGL()
{
  LOG->trace("OpenGLRender::initializeGL");

  QOpenGLFunctions::initializeOpenGLFunctions();

  // initialize OpenGL buffers
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glGenBuffers(1, &this->vbo);
  glGenBuffers(1, &this->ebo);
  glGenTextures(1, &this->texture_id);

  if (this->use_normal_map)
    glGenTextures(1, &this->normal_map_id);

  // shader program
  this->setup_shader();

  // bind
  this->shader.bind();

  this->qvao.create();
  this->qvao.bind();
  this->qvao.release();

  this->shader.release();
}

void OpenGLRender::mouseMoveEvent(QMouseEvent *event)
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

void OpenGLRender::mousePressEvent(QMouseEvent *event)
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

void OpenGLRender::on_node_compute_finished(const std::string &id)
{
  LOG->trace("OpenGLRender::on_node_compute_finished {}", id);

  if (this->p_node)
    if (id == this->p_node->get_id())
      this->set_data_again();
}

void OpenGLRender::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (this->p_node)
  {
    int port_index_elev = this->p_node->get_port_index(this->port_id_elev);

    if (p_node->get_data_ref(port_index_elev))
    {
      this->shader.bind();

      // --- transformations

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

        glm::mat4 translation_matrix = glm::translate(
            glm::mat4(1.f),
            glm::vec3(this->delta_x, this->delta_y, 0.f));

        glm::mat4 view_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -2.f));

        // define perspective projection parameters
        glm::mat4 projection_matrix = glm::perspective(glm::radians(this->fov),
                                                       this->aspect_ratio,
                                                       this->near_plane,
                                                       this->far_plane);

        glm::mat4 model_matrix = translation_matrix * rotation_matrix * scale_matrix;

        this->shader.setUniformValue(
            "model",
            QMatrix4x4(glm::value_ptr(model_matrix)).transposed());

        this->shader.setUniformValue(
            "view",
            QMatrix4x4(glm::value_ptr(view_matrix)).transposed());

        this->shader.setUniformValue(
            "projection",
            QMatrix4x4(glm::value_ptr(projection_matrix)).transposed());
      }

      if (this->wireframe_mode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      // --- eventually render the surface

      this->qvao.bind();
      glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
      this->qvao.release();

      this->shader.release();
    }
  }
}

void OpenGLRender::resizeEvent(QResizeEvent *event)
{
  QOpenGLWidget::resizeEvent(event);

  this->bind_gl_buffers();
  this->repaint();
}

void OpenGLRender::set_data(BaseNode          *new_p_node,
                            const std::string &new_port_id_elev,
                            const std::string &new_port_id_color)
{
  LOG->trace("OpenGLRender::set_data, [{}] [{}] [{}]",
             p_node ? "non-nullptr" : "nullptr",
             new_port_id_elev,
             new_port_id_color);

  this->p_node = new_p_node;
  this->port_id_elev = new_port_id_elev;
  this->port_id_color = new_port_id_color;

  if (this->p_node)
  {
    LOG->trace("OpenGLRender::set_data, p_node is NOT nullptr");

    int         port_index_elev = this->p_node->get_port_index(this->port_id_elev);
    std::string data_type_elev = this->p_node->get_data_type(port_index_elev);

    // check that data are indeed available
    if (p_node->get_data_ref(port_index_elev))
    {

      // --- elevation

      bool        elev_done = false;
      hmap::Array array;

      if (data_type_elev == typeid(hmap::Cloud).name())
      {
        LOG->trace("OpenGLRender::set_data, Cloud data");

        hmap::Cloud *p_cloud = this->p_node->get_value_ref<hmap::Cloud>(port_index_elev);
        array = hmap::Array(p_node->get_config_ref()->shape);

        if (p_cloud->get_npoints() > 0)
        {
          hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
          p_cloud->to_array(array, bbox);
        }

        elev_done = true;
      }
      else if (data_type_elev == typeid(hmap::HeightMap).name())
      {
        LOG->trace("OpenGLRender::set_data, HeightMap data");

        hmap::HeightMap *p_h = this->p_node->get_value_ref<hmap::HeightMap>(
            port_index_elev);
        array = p_h->to_array();

        elev_done = true;
      }

      if (elev_done)
      {
        // --- generate triangle mesh

        if (this->use_approx_mesh)
        {
          bool add_base = !this->wireframe_mode;
          generate_mesh_approximation(array,
                                      this->vertices,
                                      this->indices,
                                      this->max_approx_error,
                                      add_base);
        }
        else
        {
          // necessary if the model configuration is changed by the user
          if (array.size() != (int)this->vertices.size())
            generate_basemesh(array.shape, this->vertices, this->indices);

          update_vertex_elevations(array, this->vertices);
        }

        // --- color array

        bool color_done = false;

        // colorize based on the selected data (if any)
        if (this->port_id_color != "")
        {
          int port_index_color = this->p_node->get_port_index(this->port_id_color);
          std::string data_type_color = this->p_node->get_data_type(port_index_color);

          if (p_node->get_data_ref(port_index_color))
          {

            // if (data_type_color == typeid(hmap::Cloud).name())
            // {
            //   hmap::Cloud cloud = *this->p_node->get_value_ref<hmap::Cloud>(
            //       port_index_elev);
            //   hmap::Array c = hmap::Array(p_node->get_config_ref()->shape);

            //   if (cloud.get_npoints() > 0)
            //   {
            //     hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
            //     cloud.remap_values(0.5f, 1.f);
            //     cloud.to_array(c, bbox);
            //   }

            //   this->texture_diffuse = generate_selector_image(c);
            //   this->texture_shape = c.shape;

            //   hmap::apply_hillshade(this->texture_diffuse, array, 0.f, 1.f, 1.5f,
            //   true);

            //   color_done = true;
            // }
            // //
            // else

            if (data_type_color == typeid(hmap::HeightMap).name())
            {
              hmap::HeightMap *p_c = this->p_node->get_value_ref<hmap::HeightMap>(
                  port_index_color);
              hmap::Array c = p_c->to_array();

              this->texture_diffuse = generate_selector_image(c);
              this->texture_shape = p_c->shape;

              hmap::apply_hillshade(this->texture_diffuse, array, 0.f, 1.f, 1.5f, true);

              color_done = true;
            }
            //
            else if (data_type_color == typeid(hmap::HeightMapRGBA).name())
            {
              hmap::HeightMapRGBA *p_c = this->p_node->get_value_ref<hmap::HeightMapRGBA>(
                  port_index_color);

              this->texture_diffuse = p_c->to_img_8bit(p_c->shape);
              this->texture_shape = p_c->shape;

              // 'true' for RGBA, '0.9f' is exponent applied to shadows
              hmap::apply_hillshade(this->texture_diffuse, array, 0.f, 1.5f, 0.9f, true);

              color_done = true;
            }
            //
            else if (data_type_color == typeid(hmap::Path).name())
            {
              hmap::Path path = *this->p_node->get_value_ref<hmap::Path>(port_index_elev);
              hmap::Array c = hmap::Array(p_node->get_config_ref()->shape);

              if (path.get_npoints() > 0)
              {
                hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
                path.remap_values(0.5f, 1.f);
                path.to_array(c, bbox);
              }

              this->texture_diffuse = generate_selector_image(c);
              this->texture_shape = c.shape;

              hmap::apply_hillshade(this->texture_diffuse, array, 0.f, 1.f, 1.5f, true);

              color_done = true;
            }
            //
          }
        }

        // default coloring (if no data available or no coloring
        // strategy for the data type)
        if (!color_done)
        {
          this->texture_diffuse.resize(4 * array.shape.x * array.shape.y);

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
              this->texture_diffuse[k++] = (uint8_t)(255.f * hs(i, j));
              this->texture_diffuse[k++] = (uint8_t)(255.f * hs(i, j));
              this->texture_diffuse[k++] = (uint8_t)(255.f * hs(i, j));
              this->texture_diffuse[k++] = (uint8_t)(255.f);
            }

          this->texture_shape = array.shape;
        }

        // send to OpenGL buffers and update
        this->bind_gl_buffers();
      }
      else
        LOG->trace("OpenGLRender::set_data, no known rendering for type {}",
                   data_type_elev);
    }
    else
      LOG->trace("OpenGLRender::set_data, data not available for port {}",
                 this->port_id_elev);
  }
  else
  {
    LOG->trace("OpenGLRender::set_data, p_node IS nullptr");
  }

  this->repaint();
}

void OpenGLRender::set_max_approx_error(float new_max_approx_error)
{
  this->max_approx_error = new_max_approx_error;
  this->set_data_again();
}

void OpenGLRender::set_shader_type(const ShaderType &new_shader_type)
{
  this->shader_type = new_shader_type;
  this->setup_shader();
}

void OpenGLRender::set_use_approx_mesh(bool new_use_approx_mesh)
{
  this->use_approx_mesh = new_use_approx_mesh;
  this->set_data_again();
}

void OpenGLRender::set_wireframe_mode(bool new_wireframe_mode)
{
  this->wireframe_mode = new_wireframe_mode;
  this->set_data_again();
}

void OpenGLRender::setup_shader()
{
  LOG->trace("OpenGLRender::setup_shader");

  this->shader.removeAllShaders();

  char const *vs_ptr = nullptr;
  char const *gs_ptr = nullptr;
  char const *fs_ptr = nullptr;

  switch (this->shader_type)
  {
    //
  case ShaderType::NORMAL:
  {
    vs_ptr = vertex_normal.c_str();
    fs_ptr = fragment_normal.c_str();
  }
  break;
  //
  case ShaderType::TEXTURE:
  default:
  {
    vs_ptr = vertex_texture.c_str();
    fs_ptr = fragment_texture.c_str();
  }
  }

  // compile shaders
  if (!this->shader.addShaderFromSourceCode(QOpenGLShader::Vertex, vs_ptr))
  {
    LOG->critical("shader compiler (vertex) log:\n{}", this->shader.log().toStdString());
    throw std::runtime_error("Failed to compile vertex shader");
  }

  if (gs_ptr)
    if (!this->shader.addShaderFromSourceCode(QOpenGLShader::Geometry, gs_ptr))
    {
      LOG->critical("shader compiler (geometry) log:\n{}",
                    this->shader.log().toStdString());
      throw std::runtime_error("Failed to compile geometry shader");
    }

  if (!this->shader.addShaderFromSourceCode(QOpenGLShader::Fragment, fs_ptr))
  {
    LOG->critical("shader compiler (fragment) log:\n{}",
                  this->shader.log().toStdString());
    throw std::runtime_error("Failed to compile fragment shader");
  }
}

void OpenGLRender::wheelEvent(QWheelEvent *event)
{
  float dscale = this->scale * 0.1f;

  if (event->angleDelta().y() > 0)
    this->scale = std::max(0.05f, this->scale + dscale);
  else
    this->scale = std::max(0.05f, this->scale - dscale);

  this->repaint();
}

} // namespace hesiod
