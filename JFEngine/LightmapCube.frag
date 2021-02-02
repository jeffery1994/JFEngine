#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform Material material;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	//Calculate ambient light
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    //Calculate diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); 
    float diff = max(dot(norm, lightDir), 0.0); //equeals cos angle
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));  

    //Calculate specular light
    //float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos); 
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    //Combine
	vec3 result = ambient + diffuse + specular;
    //vec3 result = specular;
    FragColor = vec4(result, 1.0);
}