#version 460 core
const float gamma = 2.2;

in vec4 v_particleColor;

out vec4 fragColor;

void main()
{
    fragColor = v_particleColor;

    // final gamma correction
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / gamma));
}
