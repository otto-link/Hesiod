/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <GL/glew.h>

#include "highmap.hpp"
#include "macrologger.h"

namespace hesiod::viewer
{

//----------------------------------------
// mesh
//----------------------------------------

void generate_basemesh(hmap::Vec2<int>       shape,
                       std::vector<GLfloat> &vertices,
                       std::vector<GLfloat> &colors)
{
  size_t n_vertices = (shape.x - 1) * (shape.y - 1) * 3 * 3 * 2;
  if (vertices.size() != n_vertices)
  {
    vertices.resize(n_vertices);
    colors.resize(n_vertices);
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

void array_to_vertices(hmap::Array &array, std::vector<GLfloat> &vertices)
{
  // check vertex array size
  size_t n_vertices = (array.shape.x - 1) * (array.shape.y - 1) * 3 * 3 * 2;
  if (vertices.size() != n_vertices)
    vertices.resize(n_vertices);

  std::vector<float> x = hmap::linspace(-1.f, 1.f, array.shape.x);
  std::vector<float> y = hmap::linspace(1.f, -1.f, array.shape.y);

  int k = 0;

  for (int i = 0; i < array.shape.x - 1; i++)
    for (int j = 0; j < array.shape.y - 1; j++)
    {
      vertices[k++] = x[i];
      vertices[k++] = array(i, j);
      vertices[k++] = y[j];

      vertices[k++] = x[i];
      vertices[k++] = array(i, j + 1);
      vertices[k++] = y[j + 1];

      vertices[k++] = x[i + 1];
      vertices[k++] = array(i + 1, j + 1);
      vertices[k++] = y[j + 1];

      //
      vertices[k++] = x[i];
      vertices[k++] = array(i, j);
      vertices[k++] = y[j];

      vertices[k++] = x[i + 1];
      vertices[k++] = array(i + 1, j + 1);
      vertices[k++] = y[j + 1];

      vertices[k++] = x[i + 1];
      vertices[k++] = array(i + 1, j);
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

void update_vertex_colors(hmap::Array &z, std::vector<GLfloat> &colors)
{
  hmap::Array hs = hillshade(z, 180.f, 45.f, 10.f * z.ptp() / (float)z.shape.y);
  hs = hmap::pow(hs, 1.5f);

  int k = 0;

  for (int i = 0; i < z.shape.x - 1; i++)
    for (int j = 0; j < z.shape.y - 1; j++)
    {
      colors[k++] = hs(i, j);
      colors[k++] = hs(i, j);
      colors[k++] = hs(i, j);

      colors[k++] = hs(i, j + 1);
      colors[k++] = hs(i, j + 1);
      colors[k++] = hs(i, j + 1);

      colors[k++] = hs(i + 1, j + 1);
      colors[k++] = hs(i + 1, j + 1);
      colors[k++] = hs(i + 1, j + 1);

      colors[k++] = hs(i, j);
      colors[k++] = hs(i, j);
      colors[k++] = hs(i, j);

      colors[k++] = hs(i + 1, j + 1);
      colors[k++] = hs(i + 1, j + 1);
      colors[k++] = hs(i + 1, j + 1);

      colors[k++] = hs(i + 1, j);
      colors[k++] = hs(i + 1, j);
      colors[k++] = hs(i + 1, j);
    }
}

void update_vertex_colors(hmap::Array          &z,
                          hmap::Array          &c,
                          std::vector<GLfloat> &colors)
{
  hmap::Array hs = hillshade(z, 180.f, 45.f, 10.f * z.ptp() / (float)z.shape.y);
  hs = hmap::pow(hs, 1.5f);

  int k = 0;

  for (int i = 0; i < z.shape.x - 1; i++)
    for (int j = 0; j < z.shape.y - 1; j++)
    {
      colors[k++] = hs(i, j);
      colors[k++] = hs(i, j) * c(i, j);
      colors[k++] = hs(i, j);

      colors[k++] = hs(i, j + 1);
      colors[k++] = hs(i, j + 1) * c(i, j + 1);
      colors[k++] = hs(i, j + 1);

      colors[k++] = hs(i + 1, j + 1);
      colors[k++] = hs(i + 1, j + 1) * c(i + 1, j + 1);
      colors[k++] = hs(i + 1, j + 1);

      colors[k++] = hs(i, j);
      colors[k++] = hs(i, j) * c(i, j);
      colors[k++] = hs(i, j);

      colors[k++] = hs(i + 1, j + 1);
      colors[k++] = hs(i + 1, j + 1) * c(i + 1, j + 1);
      colors[k++] = hs(i + 1, j + 1);

      colors[k++] = hs(i + 1, j);
      colors[k++] = hs(i + 1, j) * c(i + 1, j);
      colors[k++] = hs(i + 1, j);
    }
}

void update_vertex_colors(hmap::Array          &z,
                          hmap::Array          &r,
                          hmap::Array          &g,
                          hmap::Array          &b,
                          std::vector<GLfloat> &colors)
{
  hmap::Array hs = hillshade(z, 180.f, 45.f, 10.f * z.ptp() / (float)z.shape.y);
  hs = hmap::pow(hs, 0.9f);
  hmap::remap(hs, 0.f, 1.5f);
  
  int k = 0;

  for (int i = 0; i < z.shape.x - 1; i++)
    for (int j = 0; j < z.shape.y - 1; j++)
    {
      colors[k++] = hs(i, j) * r(i, j);
      colors[k++] = hs(i, j) * g(i, j);
      colors[k++] = hs(i, j) * b(i, j);

      colors[k++] = hs(i, j + 1) * r(i, j + 1);
      colors[k++] = hs(i, j + 1) * g(i, j + 1);
      colors[k++] = hs(i, j + 1) * b(i, j + 1);

      colors[k++] = hs(i + 1, j + 1) * r(i + 1, j + 1);
      colors[k++] = hs(i + 1, j + 1) * g(i + 1, j + 1);
      colors[k++] = hs(i + 1, j + 1) * b(i + 1, j + 1);

      colors[k++] = hs(i, j) * r(i, j);
      colors[k++] = hs(i, j) * g(i, j);
      colors[k++] = hs(i, j) * b(i, j);

      colors[k++] = hs(i + 1, j + 1) * r(i + 1, j + 1);
      colors[k++] = hs(i + 1, j + 1) * g(i + 1, j + 1);
      colors[k++] = hs(i + 1, j + 1) * b(i + 1, j + 1);

      colors[k++] = hs(i + 1, j) * r(i + 1, j);
      colors[k++] = hs(i + 1, j) * g(i + 1, j);
      colors[k++] = hs(i + 1, j) * b(i + 1, j);
    }
}

//----------------------------------------
// frame buffers
//----------------------------------------

// https://www.codingwiththomas.com/blog/rendering-an-opengl-framebuffer-into-a-dear-imgui-window

// https://learnopengl.com/Advanced-OpenGL/Framebuffers

void create_framebuffer(GLuint &FBO,
                        GLuint &RBO,
                        GLuint &texture_id,
                        float   width,
                        float   height)
{
  LOG_DEBUG("%f", width);

  glDeleteFramebuffers(1, &FBO);
  glDeleteFramebuffers(1, &RBO);

  glGenFramebuffers(1, &FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGB,
               width,
               height,
               0,
               GL_RGB,
               GL_UNSIGNED_BYTE,
               NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER,
                         GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D,
                         texture_id,
                         0);

  glGenRenderbuffers(1, &RBO);
  glBindRenderbuffer(GL_RENDERBUFFER, RBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                            GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER,
                            RBO);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// here we bind our framebuffer
void bind_framebuffer(GLuint &FBO)
{
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

// here we unbind our framebuffer
void unbind_framebuffer()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// and we rescale the buffer, so we're able to resize the window
void rescale_framebuffer(GLuint &RBO,
                         GLuint &texture_id,
                         float   width,
                         float   height)
{
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGB,
               width,
               height,
               0,
               GL_RGB,
               GL_UNSIGNED_BYTE,
               NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER,
                         GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D,
                         texture_id,
                         0);

  glBindRenderbuffer(GL_RENDERBUFFER, RBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                            GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER,
                            RBO);
}

GLuint load_shaders(const char *vertex_file_path,
                    const char *fragment_file_path)
{

  // Create the shaders
  GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

  // read shader sources
  std::string vertex_shader_code;
  {
    const std::string code =
#include "vertex_shader.vs"
        ;
    vertex_shader_code = code.c_str();
  }

  std::string fragment_shader_code;
  {
    const std::string code =
#include "fragment_shader.vs"
        ;
    fragment_shader_code = code.c_str();
  }

  GLint result = GL_FALSE;
  int   info_log_length;

  // Compile Vertex Shader
  LOG_DEBUG("Compiling shader : %s", vertex_file_path);
  char const *vertex_source_pointer = vertex_shader_code.c_str();
  glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, NULL);
  glCompileShader(vertex_shader_id);

  // Check Vertex Shader
  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
  glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > 0)
  {
    std::vector<char> vertex_shader_error_message(info_log_length + 1);
    glGetShaderInfoLog(vertex_shader_id,
                       info_log_length,
                       NULL,
                       &vertex_shader_error_message[0]);
    LOG_DEBUG("%s", &vertex_shader_error_message[0]);
  }

  // Compile Fragment Shader
  LOG_DEBUG("Compiling shader : %s", fragment_file_path);
  char const *fragment_source_pointer = fragment_shader_code.c_str();
  glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, NULL);
  glCompileShader(fragment_shader_id);

  // Check Fragment Shader
  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
  glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > 0)
  {
    std::vector<char> fragment_shader_error_message(info_log_length + 1);
    glGetShaderInfoLog(fragment_shader_id,
                       info_log_length,
                       NULL,
                       &fragment_shader_error_message[0]);
    LOG_DEBUG("%s", &fragment_shader_error_message[0]);
  }

  // Link the program
  LOG_DEBUG("Linking program");
  GLuint program_id = glCreateProgram();
  glAttachShader(program_id, vertex_shader_id);
  glAttachShader(program_id, fragment_shader_id);
  glLinkProgram(program_id);

  // Check the program
  glGetProgramiv(program_id, GL_LINK_STATUS, &result);
  glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > 0)
  {
    std::vector<char> program_error_message(info_log_length + 1);
    glGetProgramInfoLog(program_id,
                        info_log_length,
                        NULL,
                        &program_error_message[0]);
    LOG_DEBUG("%s", &program_error_message[0]);
  }

  glDetachShader(program_id, vertex_shader_id);
  glDetachShader(program_id, fragment_shader_id);

  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  return program_id;
}

} // namespace hesiod::viewer
