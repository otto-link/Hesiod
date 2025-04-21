/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file open_gl_shaders.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
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

// IN
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float zmin;
uniform float zmax;
uniform float mesh_size;

uniform sampler2D textureHmap;

// OUT
out vec2 tex_coord;
out float h;

void main()
{
    h = texture(textureHmap, uv).r;
    float elevation = zmin + (zmax - zmin) * h;
    vec4 pos_dz = vec4(pos.x, elevation, pos.y, 1.0);
    gl_Position = projection * view * model * pos_dz;

    tex_coord = vec2(uv);
}
)"";

static const std::string fragment_texture = R""(
#version 330 core

// IN
in vec2 tex_coord;
in float h; // elevation in [0, 1]

uniform float mesh_size;
uniform bool use_texture_diffuse;
uniform bool use_texture_normal;
uniform bool show_normal_map;
uniform bool show_heightmap;
uniform bool show_terrain_cmap;
uniform vec3 light_dir;
uniform float shadow_saturation;
uniform float shadow_strength;
uniform float shadow_gamma;

uniform sampler2D textureDiffuse;
uniform sampler2D textureHmap;
uniform sampler2D textureNormal;

// OUT
out vec4 color;

// https://www.shadertoy.com/view/3lBXR3
vec3 turbo(float t) {
    const vec3 c0 = vec3(0.1140890109226559, 0.06288340699912215, 0.2248337216805064);
    const vec3 c1 = vec3(6.716419496985708, 3.182286745507602, 7.571581586103393);
    const vec3 c2 = vec3(-66.09402360453038, -4.9279827041226, -10.09439367561635);
    const vec3 c3 = vec3(228.7660791526501, 25.04986699771073, -91.54105330182436);
    const vec3 c4 = vec3(-334.8351565777451, -69.31749712757485, 288.5858850615712);
    const vec3 c5 = vec3(218.7637218434795, 67.52150567819112, -305.2045772184957);
    const vec3 c6 = vec3(-52.88903478218835, -21.54527364654712, 110.5174647748972);
    return c0+t*(c1+t*(c2+t*(c3+t*(c4+t*(c5+t*c6)))));
}

// https://www.shadertoy.com/view/WdjSW3
vec3 tonemap_ACES(vec3 x)
{
    // Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}

// colors from https://www.shadertoy.com/view/7ljcRW
#define CLIFF_COLOR1  vec3(0.11, 0.1, 0.1)
#define CLIFF_COLOR2  vec3(0.22, 0.2, 0.2)
#define DIRT_COLOR   vec3(0.6, 0.5, 0.4)
#define GRASS_COLOR1 vec3(0.15, 0.3, 0.1)
#define GRASS_COLOR2 vec3(0.4, 0.5, 0.2)
#define SNOW_COLOR  vec3(1.0, 1.0, 1.0)

vec2 hash( vec2 p , float fseed) // replace this by something better
{
	p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) );
	return -1.0 + 2.0*fract(sin(p + fseed)*43758.5453123);
}

float noise( in vec2 p, in float fseed )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

	vec2  i = floor( p + (p.x+p.y)*K1 );
    vec2  a = p - i + (i.x+i.y)*K2;
    float m = step(a.y,a.x);
    vec2  o = vec2(m,1.0-m);
    vec2  b = a - o + K2;
	vec2  c = a - 1.0 + 2.0*K2;
    vec3  h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
	vec3  n = h*h*h*h*vec3( dot(a,hash(i+0.0, fseed)), dot(b,hash(i+o, fseed)), dot(c,hash(i+1.0, fseed)));
    return dot( n, vec3(70.0) );
}

float fbm(in vec2 p)
{
    float n = 0.f;
    float nf = 1.f;
    float na = 0.6f;
    for (int i = 0; i < 8; i++) {
       n += noise(p * nf, 1.f) * na;
       na *= 0.5;
       nf *= 2.0;
    }
    return n;
}

void main()
{
    // compute normals
    vec2 dx = vec2(mesh_size, 0.f);
    vec2 dy = vec2(0.f, mesh_size);

    vec3 normal;

    normal.x = texture(textureHmap, tex_coord + dx).r - texture(textureHmap, tex_coord - dx).r;
    normal.y = 2.f * mesh_size;
    normal.z = texture(textureHmap, tex_coord + dy).r - texture(textureHmap, tex_coord - dy).r;

    float grad_norm = 1.f - clamp(0.05f * length(normal.xz) / mesh_size, 0.f, 1.f);

    normal = normalize(normal);


    if (use_texture_normal)
    {
        // add details
        vec3 normal_detail = texture(textureNormal, tex_coord).xyz;
        normal_detail = vec3(normal_detail.x, normal_detail.z, normal_detail.y);

        normal += normal_detail;
        normal = normalize(normal);
    }

    if (show_normal_map)
    {
        color = vec4(0.5f * normal.xzy + 0.5f, 1.f);
    }
    else
    {
        if (show_heightmap)
        {
            color = vec4(turbo(h), 1.f);
        }
        else if (use_texture_diffuse)
        {
            color = texture(textureDiffuse, tex_coord);
            // color.xyz = tonemap_ACES(color.xyz);
        }
        else if (show_terrain_cmap)
        {
            float amp = 0.1;
            float hn = h + amp * fbm(16.f * tex_coord);
            hn = clamp((hn + amp) / (1.f + 1 * amp), 0.f, 1.f);

            vec3 c3 = DIRT_COLOR;
            c3 = mix(c3, GRASS_COLOR2, smoothstep(0.f, 0.3f, hn));
            c3 = mix(c3, GRASS_COLOR1, smoothstep(0.3f, 0.6f, hn));
            c3 = mix(c3, CLIFF_COLOR1, smoothstep(0.6f, 0.8f, hn));
            c3 = mix(c3, SNOW_COLOR, smoothstep(0.8f, 1.0f, hn));

            c3 = mix(CLIFF_COLOR2, c3, grad_norm);
            c3 = tonemap_ACES(c3);

            color = vec4(c3, 1.f);
        }
        else
        {
            color = vec4(1.f, 1.f, 1.f, 1.f);
        }

        // hillshading
        float hillshade = (dot(normal, -light_dir) + 1.f - shadow_saturation) / (2.f - 2.f * shadow_saturation);
        hillshade = clamp(hillshade, 0.f, 1.f);
        hillshade = pow(hillshade, shadow_gamma);
        hillshade = (1.f - shadow_strength) + shadow_strength * hillshade;

        color *= vec4(vec3(hillshade), 1.f);
    }
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

out vec2 tex_coord;
out vec3 fragNormal;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    tex_coord = vec2(uv);
    fragNormal = normal; // mat3(transpose(inverse(model))) * normal;
}
)"";

static const std::string fragment_normal = R""(
#version 330 core

out vec4 color;
in vec2 tex_coord;
in vec3 fragNormal;

uniform sampler2D textureDiffuse;
uniform sampler2D normalMap;

void main()
{
    color = texture(normalMap, tex_coord);
}
)"";

} // namespace hesiod