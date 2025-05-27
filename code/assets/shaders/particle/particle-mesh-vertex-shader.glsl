#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_textCoord;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 offset;
uniform vec4 color;
uniform vec3 scale;

out vec4 v_particleColor;

void main()
{
    v_particleColor = color;
    vec3 spawnPos = vec3(a_pos.x * scale.x, a_pos.y * scale.y, a_pos.z * scale.z) + offset;
    gl_Position = projection * view * model * vec4(spawnPos, 1.f);
}
