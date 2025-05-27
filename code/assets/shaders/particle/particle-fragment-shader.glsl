#version 460 core
const float gamma = 2.2;

in vec2 v_texCoords;
in vec4 v_particleColor;

uniform sampler2D sprite;
uniform bool bUsesTexture;

out vec4 fragColor;

void main()
{
    if(bUsesTexture)
    {
        vec4 diffuse = texture(sprite, v_texCoords);

        if (diffuse.a < 0.1)
            discard;

        fragColor = v_particleColor * diffuse;
    }
    else
        fragColor = v_particleColor;

    fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / gamma));
}