#version 330 core

out vec4 FragColor;

//out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gSpec;
uniform vec3 viewPos;

const float gamma = 2.2;
const float exposure = 3.0;
const float shininess = 32.0;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 ApplyToneMapping(vec3 HDRValue)
{
     // reinhard tone mapping
    //vec3 mapped = HDRValue / (HDRValue + vec3(1.0));

    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-HDRValue * exposure);

    return mapped;
}

uniform Light light;

void main()
{
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 Albedo = texture(gAlbedo, TexCoords).rgb;
    vec3 Spec = texture(gSpec, TexCoords).rgb;

     // then calculate lighting as usual
    vec3 ambient = Albedo * light.ambient * 0.1; // hard-coded ambient component
    vec3 viewDir = normalize(viewPos - FragPos);

    // diffuse
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(Normal, lightDir), 0.0); //equeals cos angle
	vec3 diffuse = light.diffuse * diff * Albedo;

    //Calculate specular light
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), shininess);
    vec3 specular = light.specular * spec * Spec;

    //Combine
    vec3 Result = ambient + diffuse + specular;

    FragColor = vec4(ApplyToneMapping(Result),1.0);
    //FragColor = vec4(ambient,1.0);
}
