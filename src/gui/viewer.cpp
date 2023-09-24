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

void update_vertex_elevations(hmap::Array          &array,
                              std::vector<GLfloat> &vertices,
                              std::vector<GLfloat> &colors)
{
  hmap::Array hs =
      hillshade(array, 180.f, 45.f, 10.f * array.ptp() / (float)array.shape.y);
  hmap::clamp_min(hs, 0.f);
  hmap::remap(hs);
  hs = hmap::pow(hs, 1.5f);

  // --- elevations
  int k = 1;

  for (int i = 0; i < array.shape.x - 1; i++)
    for (int j = 0; j < array.shape.y - 1; j++)
    {
      vertices[k] = array(i, j);
      k += 3;

      vertices[k] = array(i, j + 1);
      k += 3;

      vertices[k] = array(i + 1, j + 1);
      k += 3;

      //
      vertices[k] = array(i, j);
      k += 3;

      vertices[k] = array(i + 1, j + 1);
      k += 3;

      vertices[k] = array(i + 1, j);
      k += 3;
    }

  // --- colors
  k = 0;

  for (int i = 0; i < array.shape.x - 1; i++)
    for (int j = 0; j < array.shape.y - 1; j++)
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

  // Read the Vertex Shader code from the file
  std::string   vertex_shader_code;
  std::ifstream vertex_shader_stream(vertex_file_path, std::ios::in);

  if (vertex_shader_stream.is_open())
  {
    std::stringstream sstr;
    sstr << vertex_shader_stream.rdbuf();
    vertex_shader_code = sstr.str();
    vertex_shader_stream.close();
  }
  else
  {
    printf("Impossible to open %s. Are you in the right directory ? Don't "
           "forget to read the FAQ !\n",
           vertex_file_path);
    getchar();
    return 0;
  }

  // Read the Fragment Shader code from the file
  std::string   fragment_shader_code;
  std::ifstream fragment_shader_stream(fragment_file_path, std::ios::in);
  if (fragment_shader_stream.is_open())
  {
    std::stringstream sstr;
    sstr << fragment_shader_stream.rdbuf();
    fragment_shader_code = sstr.str();
    fragment_shader_stream.close();
  }

  GLint Result = GL_FALSE;
  int   InfoLogLength;

  // Compile Vertex Shader
  printf("Compiling shader : %s\n", vertex_file_path);
  char const *VertexSourcePointer = vertex_shader_code.c_str();
  glShaderSource(vertex_shader_id, 1, &VertexSourcePointer, NULL);
  glCompileShader(vertex_shader_id);

  // Check Vertex Shader
  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0)
  {
    std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
    glGetShaderInfoLog(vertex_shader_id,
                       InfoLogLength,
                       NULL,
                       &VertexShaderErrorMessage[0]);
    printf("%s\n", &VertexShaderErrorMessage[0]);
  }

  // Compile Fragment Shader
  printf("Compiling shader : %s\n", fragment_file_path);
  char const *FragmentSourcePointer = fragment_shader_code.c_str();
  glShaderSource(fragment_shader_id, 1, &FragmentSourcePointer, NULL);
  glCompileShader(fragment_shader_id);

  // Check Fragment Shader
  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0)
  {
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
    glGetShaderInfoLog(fragment_shader_id,
                       InfoLogLength,
                       NULL,
                       &FragmentShaderErrorMessage[0]);
    printf("%s\n", &FragmentShaderErrorMessage[0]);
  }

  // Link the program
  printf("Linking program\n");
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, vertex_shader_id);
  glAttachShader(ProgramID, fragment_shader_id);
  glLinkProgram(ProgramID);

  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0)
  {
    std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
    glGetProgramInfoLog(ProgramID,
                        InfoLogLength,
                        NULL,
                        &ProgramErrorMessage[0]);
    printf("%s\n", &ProgramErrorMessage[0]);
  }

  glDetachShader(ProgramID, vertex_shader_id);
  glDetachShader(ProgramID, fragment_shader_id);

  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  return ProgramID;
}

} // namespace hesiod::viewer
