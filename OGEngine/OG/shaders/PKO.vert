#version 450 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec3 vNormal;

out VS_OUT
{
    vec3 fragNormal;
    vec3 fragPos;
    vec3 lightPos;
} vs_out;

layout(std140) uniform Transforms
{
    mat4 view;
    mat4 projection;
};

uniform mat4 model;
uniform mat3 normalMatrix;
uniform vec3 lightPos;

void main()
{    
    vs_out.fragNormal = normalMatrix * vNormal; 
    vs_out.fragPos = (view * model * vec4(vPos, 1.0)).xyz;
    vs_out.lightPos = (view * vec4(lightPos, 1.0)).xyz;
    gl_Position = projection * vec4(vs_out.fragPos, 1.0);
}