#version 450 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec3 vNormal;

layout(std140) uniform Transforms
{
    mat4 view;
    mat4 projection;
};

uniform mat4 model;

void main()
{    
    gl_Position = projection * view * model * vec4(vPos, 1.0);
}