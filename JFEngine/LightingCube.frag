#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    //Calculate diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); 
    float diff = max(dot(norm, lightDir), 0.0); //equeals cos angle
    vec3 diffuse = diff * lightColor;

    //Calculate ambient light
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    //Calculate specular light
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos); 
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

    //Combine ambient and diffuse
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}