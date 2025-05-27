#version 460 core

in vec3 v_position;
in vec3 v_color;
in vec3 v_normal;

const vec3 lightPos = vec3(5, 1.0, -1.0);
const float shininess = 4.0;
const float ka = 0.5;
const float kd = 0.6;
const float ks = 0.4;

out vec4 fragColor;

void main()
{
   vec3 s = normalize(lightPos - v_position);
   vec3 r = reflect(-s, normalize(v_normal));

   vec3 specular = v_color * ks * pow(max(0.0, dot(r, normalize(-v_position))), shininess);
   vec3 ambient = v_color * ka;
   vec3 diffuse = v_color * kd * max(0.0, dot(normalize(v_normal), s));

   vec3 sum = specular + ambient + diffuse;
   fragColor = vec4(sum, 1.0);
}