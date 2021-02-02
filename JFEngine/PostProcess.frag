#version 330 core

layout (location = 0) out vec4 FragColor;

//out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int PostProcessEffect;

const float offset = 1.0 / 300.0;  
const float gamma = 2.2;
const float exposure = 1.0;

vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

float Sharpenkernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

float Blurkernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

float EdgeDetectionKernel[9] = float[]
(
    1,      1, 1,
    1,     -8, 1,
    1,      1, 1
);

vec3 CalculateKernalEffect(float KernelEffect[9])
{
    vec3 sampleTex[9];
     for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
     for(int i = 0; i < 9; i++)
        col += sampleTex[i] * KernelEffect[i];
    return col;
}

vec3 ApplyToneMapping(vec3 HDRValue)
{
     // reinhard tone mapping
    //vec3 mapped = HDRValue / (HDRValue + vec3(1.0));

    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-HDRValue * exposure);

    return mapped;
}

void main()
{
    vec4 Result = vec4(0.1, 0.1, 0.1, 1.0);
    vec3 col = texture(screenTexture, TexCoords).rgb;
    switch(PostProcessEffect)
    {
    case 0:
    default:
        //Result = vec4(col, 1.0);
        Result = texture(screenTexture, TexCoords);
        break;
    case 1:
        Result = vec4(vec3(1.0 - col), 1.0);
        break;
    case 2:
        float average = (col.r + col.g + col.b) / 3.0;
        Result = vec4(average, average, average, 1.0);
        break;
    case 3:
        float average2 = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
        Result = vec4(average2, average2, average2, 1.0);
        break;
    case 4:
        Result = vec4(CalculateKernalEffect(Sharpenkernel), 1.0);
        break;
    case 5:
        Result = vec4(CalculateKernalEffect(Blurkernel), 1.0);
        break;
    case 6:
        Result = vec4(CalculateKernalEffect(EdgeDetectionKernel), 1.0);
        break;
    case 7:
        float depthValue = texture(screenTexture, TexCoords).r;
        Result = vec4(vec3(depthValue), 1.0);
    case 8:
        Result = vec4(ApplyToneMapping(col),1.0);
        break;
    case 9:
        Result = vec4(ApplyToneMapping(col),1.0);
        break;
    }
    if(PostProcessEffect != 7)
    {
        Result.rgb = pow(Result.rgb, vec3(1.0/gamma));
    }
    FragColor = Result;
}
