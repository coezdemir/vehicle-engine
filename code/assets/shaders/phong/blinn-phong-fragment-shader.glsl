#version 460 core
struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_emissive1;
    bool bShouldUseTextures;
    bool bHasDiffuseMap;
    bool bHasSpecularMap;
    bool bHasNormalMap;
    bool bHasEmissiveMap;
    bool bFlunctuateEmission;
    float flunctuationInterval;
    float flunctuationDelay;
};

struct Light
{
    vec4 vector;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform float time;

const float gamma = 2.2;

in vec3 v_position;
in vec3 v_normal;
in vec2 v_textCoord;
in vec3 v_tangentLightPos;
in vec3 v_tangentViewPos;
out vec4 fragColor;

void main()
{
    vec3 normal = material.bHasNormalMap? texture(material.texture_normal1, v_textCoord).rgb : v_normal;
    normal = material.bHasNormalMap? normalize(normal * 2.0 - 1.0) : normal;

    // ambient
    vec3 ambient = light.ambient *
                    (material.bHasDiffuseMap ?
                    vec3(pow(texture(material.texture_diffuse1, v_textCoord).rgb, vec3(gamma))) : material.ambient);

    // diffuse
    bool bIsPointLight = light.vector.w != 0;
    vec3 lightDir = bIsPointLight ? normalize(v_tangentLightPos - v_position) : normalize(v_tangentLightPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff *
                    (material.bHasDiffuseMap?
                    vec3(pow(texture(material.texture_diffuse1, v_textCoord).rgb, vec3(gamma))) : material.diffuse);

    // specular
    vec3 viewDir = normalize(v_tangentViewPos - v_position);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec *
                    (material.bHasSpecularMap?
                    vec3(texture(material.texture_specular1, v_textCoord)) : material.specular);

    // emission
    vec3 emission = vec3(0.0);
    if (material.bHasEmissiveMap)
    {
        emission = texture(material.texture_emissive1, v_textCoord).rgb;

        if(material.bFlunctuateEmission)
            emission = emission * (sin(time * material.flunctuationInterval + material.flunctuationDelay) * 0.5 + 0.5) * 2.0;
    }

    vec3 result = ambient + diffuse + specular + emission;
    fragColor = vec4(result, 1.0);

    // final gamma correction
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / gamma));
}
