#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gSpec;
  
in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D shadowMap;
uniform samplerCube skybox;
uniform float shininess;
uniform bool hasLight;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform vec3 viewPos;

void main()
{
    // store the fragment position vector in the first gbuffer texture
    //gPosition = FragPos;
    gPosition = vec4(FragPos, 1.0);
    // also store the per-fragment normals into the gbuffer
    gNormal = vec4(normalize(Normal), 1.0);
    // and the diffuse per-fragment color
    gAlbedo = texture(texture_diffuse1, TexCoord);
    // store specular intensity in gAlbedoSpec's alpha component
    gSpec = texture(texture_specular1, TexCoord);
}