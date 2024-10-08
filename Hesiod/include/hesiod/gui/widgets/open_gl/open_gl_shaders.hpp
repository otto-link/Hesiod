/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file open_gl_shaders.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

namespace hesiod
{

enum ShaderType : int
{
  NORMAL,
  TEXTURE,
};

struct ShaderConfig
{
  std::string label;
  std::string vertex_src;
  std::string geometry_src;
  std::string fragment_src;
  bool        use_normal_map;
};

// static const shader_texture

// --- texture

static const std::string vertex_texture = R""(
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    texCoord = vec2(uv);
}
)"";

static const std::string fragment_texture = R""(
#version 330 core

out vec4 color;
in vec2 texCoord;

uniform sampler2D textureDiffuse;
uniform sampler2D normalMap;

void main()
{
    color = texture(textureDiffuse, texCoord);
}
)"";

// --- normal

static const std::string vertex_normal = R""(
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;
out vec3 fragNormal;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    texCoord = vec2(uv);
    fragNormal = normal; // mat3(transpose(inverse(model))) * normal;
}
)"";

static const std::string fragment_normal = R""(
#version 330 core

out vec4 color;
in vec2 texCoord;
in vec3 fragNormal;

uniform sampler2D textureDiffuse;
uniform sampler2D normalMap;

void main()
{
    color = texture(normalMap, texCoord);
}
)"";

} // namespace hesiod