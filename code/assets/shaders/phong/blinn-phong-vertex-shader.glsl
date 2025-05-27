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

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_textCoord;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_textCoord;
out vec3 v_tangentLightPos;
out vec3 v_tangentViewPos;

void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * a_tangent);
    vec3 N = normalize(normalMatrix * a_normal);
    //vec3 B = normalize(normalMatrix * a_bitangent);

    // Gram-Schmidt process -> re-orthogonalization
    T = normalize(T - dot(T, N) * N);

    vec3 B = cross(T, N);

    mat3 TBN = transpose(mat3(T, B, N));
    v_tangentLightPos = TBN * vec3(light.vector);
    v_tangentViewPos  = TBN * viewPos;

    v_position = TBN * vec3(model * vec4(a_pos, 1.0));
    v_normal = a_normal;
    v_textCoord = a_textCoord;

    gl_Position = projection * view * model * vec4(a_pos, 1.f);
}
