///////////////////////////////////////////////////////////////////////////////
//
// General blinn-phone lighting shader
// @Jianfei Zhou
//
///////////////////////////////////////////////////////////////////////////////
#version 330 core

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;
in vec4 FragPosLightSpace;

///////////////////////////////////////////////////////////////////////////////
//
//This is a general kind of light, regradless of its type, its always
//have all attributes of all types of light casters.
//
///////////////////////////////////////////////////////////////////////////////
struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOffAngleCosine;
    float outterCutoff;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const int MAX_NUM_OF_LIGHT = 1;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D shadowMap;
uniform samplerCube skybox;
uniform float shininess;
uniform bool hasLight;
uniform int numberOfLights;
uniform Light[MAX_NUM_OF_LIGHT] lights;
uniform vec3 viewPos;

///////////////////////////////////////////////////////////////////////////////
//
//	We are passing in the position of current fragment in light space
//	This function is used to calculate shadow from directional light
//
///////////////////////////////////////////////////////////////////////////////
float ShadowCalculation(vec4 fragPosLightSpace, Light light)
{
    //Maually doing perspective divided, When we are using othroprojection,
    //this statement is useless. But this is a must in terms of perspective projection.
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    //transform this fragment coordinates from normal device coordinates to [0,1]
    //Which is the same as depth value in depth map
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.01 * (1.0 - dot(Normal, light.direction)), 0.001);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    return shadow;
}

///////////////////////////////////////////////////////////////////////////////
//
// Calculate light from point light
//
///////////////////////////////////////////////////////////////////////////////
vec3 CalculatePointLight(Light light)
{
    vec3 lightDir = normalize(light.position - FragPos);

    //Calculate ambient light
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoord));

    // diffuse 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoord).rgb;  

    // specular
    vec3 viewDir = normalize(viewPos - FragPos); 
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), shininess);
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoord));

    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    diffuse   *= attenuation;
    specular *= attenuation;   

    //vec3 result = ambient + (1.0) * (diffuse + specular);
    vec3 result = diffuse + specular + ambient;
    return result;
}

///////////////////////////////////////////////////////////////////////////////
//
// Calculate light from spot light
//
///////////////////////////////////////////////////////////////////////////////
vec3 CalculateSpotLight(Light light)
{
    vec3 lightDir = normalize(light.position - FragPos);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOffAngleCosine - light.outterCutoff;
    float intensity = clamp((theta - light.outterCutoff) / epsilon, 0.0, 1.0); 

    //Calculate ambient light
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoord));

    vec3 result = ambient;

    // diffuse 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoord).rgb;  

    // specular
    vec3 viewDir = normalize(viewPos - FragPos); 
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), shininess);
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoord));
    diffuse *= intensity;
    specular *= intensity;

    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    diffuse   *= attenuation;
    specular *= attenuation;
    result = result + diffuse + specular;
    //result = diffuse;
    
    return result;
}

///////////////////////////////////////////////////////////////////////////////
//
// Calculate light from directional light
//
///////////////////////////////////////////////////////////////////////////////
vec3 CalculateDirctionalLight(Light light)
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
    //vec3 reflectDir = reflect(-lightDir, norm);  
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), shininess);
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoord));
    //flat spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    //vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoord));

    float shadow = ShadowCalculation(FragPosLightSpace, light);
    //Combine
    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);
    return result;
}

///////////////////////////////////////////////////////////////////////////////
//
// Combine all lights and shadow together with forward rendering
//
///////////////////////////////////////////////////////////////////////////////
void main()
{
    vec3 Result = vec3(0.0, 0.0, 0.0);
    if(hasLight)
    {
        for(int idx = 0; idx < numberOfLights; ++idx)
        {
            if(lights[idx].type == 0)
            {
                Result += CalculateDirctionalLight(lights[idx]);
            }
            else if(lights[idx].type == 1)
            {
                //Result = vec3(1.0, 0.0, 0.0);
                Result += CalculatePointLight(lights[idx]);
            }
            else if(lights[idx].type == 2)
            {
				//Result = vec3(1.0, 0.0, 0.0);
                Result += CalculateSpotLight(lights[idx]);
            }
        }
    }
    else
    {
        vec4 textureColor = texture(texture_diffuse1, TexCoord);
        if(textureColor.a < 0.1)
        {
            discard;
        }
        Result = textureColor.rgb;
    }
    FragColor = vec4(Result, 1.0);
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.12)
    {
        BrightColor = vec4(FragColor.rgb, 1.0);
    }
    else
    {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);   
    }
}