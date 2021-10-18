#version 450 core

out vec4 fragColor;

in vec3  lightPos;

in VS_OUT
{
    vec3 fragNormal;
    vec3 fragPos;
    vec3 lightPos;
} fs_in;

layout(std140) uniform LightInfo
{
    vec3 Color;
    float ambientStrength; //Ia
    float diffuseStrength; //Id
    float specularStrength; //Is
} Light;

// Need Material Ka,Kd,Ks,Shininess
layout(std140) uniform Material
{
    float Ka;
    float Kd;
    float Ks;
    float shininess;
};


uniform vec3 objectColor;

void main() {

    vec3 ambient = Light.ambientStrength * Light.Color;

    vec3 norm = normalize(fs_in.fragNormal);
    vec3 lightDir = normalize(fs_in.lightPos - fs_in.fragPos);  
    float diffuse = max(dot(norm, lightDir), 0.0);


    vec3 result = (ambient + diffuse) * objectColor;

    fragColor = vec4(result, 1.0);
}