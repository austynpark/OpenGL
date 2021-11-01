#version 450 core

out vec4 fragColor;

in VS_OUT
{
    vec3 fragNormal;
    vec3 fragPos;
    vec3 lightPos;
} fs_in;

void main() {

    vec3 norm = normalize(fs_in.fragNormal);
    vec3 lightDir = normalize(fs_in.lightPos - fs_in.fragPos);  
    float diffuse = max(dot(norm, lightDir), 0.0);

    vec3 result = diffuse * vec3(1.0);
    fragColor = vec4(result, 1.0);
}