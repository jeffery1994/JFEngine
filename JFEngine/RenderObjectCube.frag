#version 330 core
out vec4 FragColor;
  
in vec4 ourColor;
in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float shininess;

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
	//Calculate ambient light
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoord));

	 //Calculate diffuse light
    vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0); //equeals cos angle
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoord));  

	//Calculate specular light
    vec3 viewDir = normalize(viewPos - FragPos); 
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoord));

	//Combine
    vec3 result = ambient + specular + diffuse;
	//vec3 result = specular;
	FragColor = vec4(result, 1.0);
}