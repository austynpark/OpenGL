#version 450 core

out vec4 fragColor;

in VS_OUT
{
    vec3 fragNormal;
    vec3 fragPos;
} fs_in;

uniform struct LightInfo
{
    vec3 Pos;
    vec3 Color;
    float ambientStrength;
} Light;

uniform vec3 objectColor;

void main() {

    vec3 ambient = Light.ambientStrength * Light.Color;

    vec3 norm = normalize(fs_in.fragNormal);
    vec3 lightDir = normalize(Light.Pos - fs_in.fragPos);  
    float diffuse = max(dot(norm, lightDir), 0.0);

    vec3 result = (ambient + diffuse) * objectColor;

    fragColor = vec4(result, 1.0);
}