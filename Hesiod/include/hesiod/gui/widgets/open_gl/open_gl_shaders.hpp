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
  TEXTURE,
};

// --- texture

static const std::string vertex_texture = R""(
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;

uniform mat4 modelMatrix;
out vec2 texCoord;

void main(){
    gl_Position = modelMatrix * vec4(pos, 1.0);
    texCoord = vec2(uv);
}
)"";

static const std::string fragment_texture = R""(
#version 330 core

out vec4 color;
in vec2 texCoord;

uniform sampler2D textureSampler;

void main()
{
    color = texture(textureSampler, texCoord);
}
)"";

} // namespace hesiod