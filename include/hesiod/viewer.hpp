/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <GL/glew.h>

#include "highmap.hpp"

namespace hesiod::viewer
{

//----------------------------------------
// mesh
//----------------------------------------

void generate_basemesh(hmap::Vec2<int>       shape,
                       std::vector<GLfloat> &vertices,
                       std::vector<GLfloat> &colors);

void array_to_vertices(hmap::Array &array, std::vector<GLfloat> &vertices);

void update_vertex_elevations(hmap::Array          &array,
                              std::vector<GLfloat> &vertices,
                              std::vector<GLfloat> &colors);

//----------------------------------------
// frame buffers
//----------------------------------------

// https://www.codingwiththomas.com/blog/rendering-an-opengl-framebuffer-into-a-dear-imgui-window

void create_framebuffer(GLuint &FBO,
                        GLuint &RBO,
                        GLuint &texture_id,
                        float   width,
                        float   height);

// here we bind our framebuffer
void bind_framebuffer(GLuint &FBO);

// here we unbind our framebuffer
void unbind_framebuffer();

// and we rescale the buffer, so we're able to resize the window
void rescale_framebuffer(GLuint &RBO,
                         GLuint &texture_id,
                         float   width,
                         float   height);

GLuint load_shaders(const char *vertex_file_path,
                    const char *fragment_file_path);

} // namespace hesiod::viewer
