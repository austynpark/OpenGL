#version 450 core

out vec4 fragColor;
in vec2 TexCoords;

uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;
uniform sampler2D gbuffer_diffuse;
uniform sampler2D gbuffer_specular;

uniform vec3 att;
uniform vec3 global_ambient;
uniform vec3 fog_color;
uniform float near;
uniform float far;
uniform int numOfLights;


struct Light
{
	vec3 Ia;
    vec3 Id;
    vec3 Is;
    vec3 position; 
    vec3 direction;
    float inner_cone;
    float outer_cone;
    float fall_off;
	int type;
};

layout(std140) uniform LightInfo
{
    Light light[16];
};

vec3 CalcDirLight(Light light, vec3 pos,vec3 norm, vec3 Kd, vec3 Ks);
vec3 CalcPointLight(Light light, vec3 pos, vec3 norm, vec3 Kd, vec3 Ks);
vec3 CalcSpotLight(Light light, vec3 pos, vec3 norm, vec3 Kd, vec3 Ks);

void main()
{
    vec3 pos = texture(gbuffer_position, TexCoords).rgb;
    vec3 norm = texture(gbuffer_normal, TexCoords).rgb;
    vec3 Kd = texture(gbuffer_diffuse, TexCoords).rgb;
    vec3 Ks = texture(gbuffer_specular, TexCoords).rgb;

    vec3 result = vec3(0.0f);

    for(int i = 0; i < numOfLights; ++i) {
        if(light[i].type == 1)
            result += CalcSpotLight(light[i], pos, norm, Kd, Ks);
        else if(light[i].type == 2)
            result += CalcDirLight(light[i], pos, norm, Kd, Ks);
        else
            result += CalcPointLight(light[i], pos, norm, Kd, Ks);
    }

    result += global_ambient * Kd;

    float dist = length(-pos);
    float fogFactor = (far-dist)/(far-near);

    fogFactor = clamp(fogFactor, 0.0, 1.0);

    result = mix(fog_color, result, fogFactor);
    result = clamp(result, 0, 1);

    fragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(Light light, vec3 pos, vec3 norm, vec3 Kd, vec3 Ks) {
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
            specular = pow(max(dot(viewDir, reflectDir), 0.0), Ks.r * Ks.r * 32);
    }

	vec3 I_ambient = light.Ia * Kd;
    vec3 I_diffuse = light.Id * diff * Kd;
    vec3 I_specular = light.Is * Ks.rgb * specular;

    return (I_ambient + I_diffuse + I_specular);

}
vec3 CalcPointLight(Light light, vec3 pos, vec3 norm, vec3 Kd, vec3 Ks) {

    vec3 lightDir = normalize(light.position - pos);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(-pos);

    float specular = 0.0f;

    if(diff > 0.0)
    {
        vec3 reflectDir = 2 * dot(norm, lightDir) * norm - lightDir;
        float spec = max(dot(viewDir, reflectDir), 0.0);
        if(spec != 0)
            specular = pow(max(dot(viewDir, reflectDir), 0.0), Ks.r * Ks.r * 32);
    }

    float dist = length(light.position - pos);
    float attenuation = min(1.0 / (att.x + att.y * dist + att.z * (dist * dist)), 1);

	vec3 I_ambient = light.Ia * Kd;
    vec3 I_diffuse = light.Id * diff * Kd;
    vec3 I_specular = light.Is * Ks.xyz * specular;
    
    return (I_ambient + I_diffuse + I_specular) * attenuation;
}
vec3 CalcSpotLight(Light light, vec3 pos, vec3 norm, vec3 Kd, vec3 Ks) {

    float theta = radians(light.inner_cone);
    float phi = radians(light.outer_cone);

    // spot light unit vector lightDir = (L), light.direction = (D)
    vec3 lightDir = normalize(light.position - pos);
    
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(-pos);

    float specular = 0.0f;
    // specular shading
    if(diff > 0)
    {
	    vec3 reflectDir = 2 * dot(norm, lightDir) * norm - lightDir;
        float spec = max(dot(viewDir, reflectDir), 0.0);
        if(spec != 0)
            specular = pow(max(dot(viewDir, reflectDir), 0.0), Ks.r * Ks.r * 32);
    }

    // attenuation
    float dist = length(light.position - pos);
    float attenuation = min(1.0 / (att.x + att.y * dist + att.z * (dist * dist)), 1);    
    
    float LdotD = max(dot(-lightDir, normalize(light.direction)), 0.0);
    float intensity = 0;

    if(LdotD > cos(theta))
    {
        intensity = 1;
    }
    else if(LdotD >= cos(phi))
    {
        intensity = pow(
            (LdotD - cos(phi)) / (cos(theta) - cos(phi))
            ,  light.fall_off);
    }

    vec3 I_diffuse = light.Id * diff * Kd;
    vec3 I_specular = light.Is * Ks.xyz * specular;
    
    return (I_diffuse + I_specular) * attenuation * intensity;
}