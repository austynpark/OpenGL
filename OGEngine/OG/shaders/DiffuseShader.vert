#version 400 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec3 vNormal;

vec3 diffuseColor = vec3( 0.3f, 0.3f, 0.3f );

layout(std140) uniform Transforms
{
    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 normalMatrix;
} vs_transform;

out VS_OUT
{
    vec3 fragDiffuse;
    vec3 fragNormal;
    vec3 fragPos;

} vs_out;

void main()
{
    vs_out.fragDiffuse = diffuseColor;
    vs_out.fragNormal = vs_transform.normalMatrix * vertexNormal;
    vs_out.fragPos = (vs_transform.view * vs_transform.model * vec4(vPos, 1.0)).xyz;


	gl_Position = vs_transform.projection * vec4( vs_out.fragPos, 1.0f );
}
