#version 460 core
layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_textCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 offset;
uniform vec4 color;
uniform vec3 scale;

out vec2 v_texCoords;
out vec4 v_particleColor;

void main()
{
    v_texCoords = a_textCoord;
    v_particleColor = color;
    vec3 spawnPos = vec3(a_position.x * scale.x, a_position.y * scale.y, 0.f)  + offset;
    gl_Position = projection * view * model * vec4(spawnPos, 1.f);
}
