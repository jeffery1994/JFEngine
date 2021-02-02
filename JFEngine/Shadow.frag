#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture_diffuse1;

void main()
{             
    gl_FragDepth = gl_FragCoord.z;
    FragColor = vec4(texture(texture_diffuse1, TexCoord));
}
