#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec2 TexCoord;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0f);
    TexCoord = aTexCoord;
    //gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0f);
}