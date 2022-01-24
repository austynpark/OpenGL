#version 450 core

out vec4 fragColor;
in vec2 TexCoords;

uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;
uniform sampler2D gbuffer_material;
uniform sampler2D gbuffer_depth;

struct Light
{
    float inner_cone;
    float outer_cone;
    float fall_off;
	int type;
	vec3 Ia;
    vec3 Id;
    vec3 Is;
    vec3 position; 
    vec3 direction;
};

layout(std140) uniform LightInfo
{
    Light light[16];
	vec3 att;
    vec3 global_ambient;
    vec3 fog_color;
    float near;
    float far;
    int numOfLights;
};

vec3 CalcDirLight(Light light, vec3 pos,vec3 norm, vec4 mat, float depthFromEye);
vec3 CalcPointLight(Light light, vec3 pos, vec3 norm, vec4 mat, float depthFromEye);
vec3 CalcSpotLight(Light light, vec3 pos, vec3 norm, vec4 mat, float depthFromEye);

void main()
{
    vec3 pos = texture(gbuffer_position, TexCoords).rgb;
    vec3 norm = texture(gbuffer_normal, TexCoords).rgb;
    vec4 material = texture(gbuffer_material, TexCoords);
    float depthFromEye = texture(gbuffer_depth, TexCoords).r;
    vec3 result = vec3(0.0f);

    for(int i = 0; i < numOfLights; ++i) {
        if(light[i].type == 1)
            result += CalcSpotLight(light[i], pos, norm, material, depthFromEye);
    }

}

vec3 CalcDirLight(Light light, vec3 pos, vec3 norm, vec4 mat, float depthFromEye) {
    // Object to Light
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(-pos);

    float specular = 0.0f;

    if(diff > 0.0)
    {
		vec3 reflectDir = 2 * dot(norm, lightDir) * norm - lightDir;
        float spec = max(dot(viewDir, reflectDir), 0.0);
        if(spec != 0)
            specular = pow(max(dot(viewDir, reflectDir), 0.0), mat.a * mat.a * 32);
    }

	vec3 I_ambient = light.Ia * mat.rgb;
    vec3 I_diffuse = light.Id * diff * mat.rgb;
    vec3 I_specular = light.Is * mat.a;

    return (I_ambient + I_diffuse + I_specular);

}
vec3 CalcPointLight(Light light, vec3 pos, vec3 norm, vec4 mat, float depthFromEye) {
    return vec3(0.0);
}
vec3 CalcSpotLight(Light light, vec3 pos, vec3 norm, vec4 mat, float depthFromEye) {
    return vec3(0.0);
}