#version 460 core

in vec3 a_position;
in vec3 a_normal;
in vec3 a_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normal;

out vec3 v_position;
out vec3 v_normal;
out vec3 v_color;


void main()
{
   gl_Position = projection * view * model * vec4(a_position, 1.0f);

   v_position = (view * model * vec4(a_position, 1.0f)).xyz;
   v_color = a_color;
   v_normal = (normal * vec4(a_normal, 0.0)).xyz;
}