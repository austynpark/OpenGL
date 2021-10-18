#version 450 core
layout (location = 0) in vec3 vPos;

uniform mat4 model;

layout(std140) uniform Transforms
{
    mat4 view;
    mat4 projection;
};
void main()
{    
    gl_Position = projection * view * model * vec4(vPos, 1.0);
}