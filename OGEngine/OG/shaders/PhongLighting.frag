#version 450 core

out vec4 fragColor;

in VS_OUT
{
    vec3 fragNormal;
    vec3 fragPos;
    vec3 lightPos;
    vec2 TexCoords;
} fs_in;

layout(std140) uniform LightInfo
{
    vec3 ambient; //Ia
    vec3 diffuse; //Id
    vec3 specular; //Is
} Light;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;
uniform float shininess;
//uniform vec3 objectColor;

void main() {

    vec3 ambient = Light.ambient * texture(diffuse_texture, fs_in.TexCoords).rgb;

    vec3 norm = normalize(fs_in.fragNormal);
    vec3 lightDir = normalize(fs_in.lightPos - fs_in.fragPos);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = Light.diffuse * diff * texture(diffuse_texture, fs_in.TexCoords).rgb;

    vec3 viewDir = normalize(-fs_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = Light.specular * spec * texture(specular_texture, fs_in.TexCoords).rgb;

    vec3 result = (ambient + diff + specular);

    fragColor = vec4(result, 1.0);
}
