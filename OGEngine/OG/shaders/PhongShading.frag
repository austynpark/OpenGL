#version 450 core

out vec4 fragColor;

in VS_OUT
{
    vec3 fragNormal;
    vec3 fragPos;
    vec2 TexCoords;
    vec3 Pos;
    vec3 Norm;
} fs_in;

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
    vec3 camera;
    vec3 global_ambient;
    vec3 fog_color;
    float near;
    float far;
    int numOfLights;
};

uniform bool isNormMapping;

vec3 CalcDirLight(Light light, vec3 norm, vec3 viewDir, vec2 texCoords) 
{
    // Object to Light
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(norm, lightDir), 0.0);

    vec4 Ks = vec4(1.0f);
    vec3 Kd = vec3(1.0f);

    if(uvType != 4)
    {
        Ks = texture(specular_texture, texCoords);
        Kd = vec3(texture(diffuse_texture, texCoords));
    }

    float specular = 0;

    if(diff > 0.0)
    {
		vec3 reflectDir = 2 * dot(norm, lightDir) * norm - lightDir;
        float spec = max(dot(viewDir, reflectDir), 0.0);
        if(spec != 0)
            specular = pow(max(dot(viewDir, reflectDir), 0.0), Ks.r * Ks.r * 32);
    }

	vec3 I_ambient = light.Ia * Kd;
    vec3 I_diffuse = light.Id * diff * Kd;
    vec3 I_specular = light.Is * Ks.xyz * specular;

    return (I_ambient + I_diffuse + I_specular);
}

vec3 CalcPointLight(Light light, vec3 norm, vec3 viewDir, vec2 texCoords)
{
    vec3 lightDir = normalize(light.position - fs_in.fragPos);

    float diff = max(dot(norm, lightDir), 0.0);

    vec4 Ks = vec4(1.0f);
    vec3 Kd = vec3(1.0f);

    if(uvType != 4)
    {
        Ks = texture(specular_texture, texCoords);
        Kd = vec3(texture(diffuse_texture, texCoords));
    }

    float specular = 0.0f;

    if(diff > 0.0)
    {
        vec3 reflectDir = 2 * dot(norm, lightDir) * norm - lightDir;
        float spec = max(dot(viewDir, reflectDir), 0.0);
        if(spec != 0)
            specular = pow(max(dot(viewDir, reflectDir), 0.0), Ks.r * Ks.r * 32);
    }

    float dist = length(light.position - fs_in.fragPos);
    float attenuation = min(1.0 / (att.x + att.y * dist + att.z * (dist * dist)), 1);

	vec3 I_ambient = light.Ia * Kd;
    vec3 I_diffuse = light.Id * diff * Kd;
    vec3 I_specular = light.Is * Ks.xyz * specular;
    
    return (I_ambient + I_diffuse + I_specular) * attenuation;
}

vec3 CalcSpotLight(Light light, vec3 norm, vec3 viewDir, vec2 texCoords)
{
    float theta = radians(light.inner_cone);
    float phi = radians(light.outer_cone);

    // spot light unit vector lightDir = (L), light.direction = (D)
    vec3 lightDir = normalize(light.position - fs_in.fragPos);
    
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec4 Ks = vec4(1.0f);
    vec3 Kd = vec3(1.0f);

    if(uvType != 4)
    {
        Ks = texture(specular_texture, texCoords);
        Kd = vec3(texture(diffuse_texture, texCoords));
    }

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
    float dist = length(light.position - fs_in.fragPos);
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

vec2 CalcCubeMap(vec3 vEntity)
{
        float x = vEntity.x;
        float y = vEntity.y;
        float z = vEntity.z;

        float absX = abs(x);
        float absY = abs(y);
        float absZ = abs(z);

        vec2 uv = vec2(0.0);

        // POSITIVE & NEGATIVE X
        if ((absX >= absY) && (absX >= absZ))
        {
			uv.y = -y / absX;

            if (x > 0)
            {
                uv.x = -z / absX;
            }
            else
            {
                uv.x = z / absX;
            }
        }
        // POSITIVE & NEGATIVE Y
        else if ((absY >= absX) && (absY >= absZ))
        {
            uv.x = x / absY;

            if (y > 0)
            {
                uv.y = z / absY;
            }
            else
            {
                uv.y = -z / absY;
            }
        }
        // POSITIVE & NEGATIVE Z
        else if (absZ >= absX && absZ >= absY)
        {
			uv.y = -y / absZ;

            if (z > 0)
            {
                uv.x = x / absZ;
            }
            else
            {
                uv.x = -x / absZ;
            }
        }

        // Convert range from -1 to 1 to 0 to 1
        return (uv + vec2(1.0)) * 0.5f;
}

vec3 CalcRefract(vec3 I, vec3 N, float eta) {
    
    float NdotI = dot(N, I);

    /*
    if(NdotI < 0) {
        NdotI = -NdotI;
    } else {
        N = -N;
        eta = 1 / eta;
    }
    */

    float k = 1.0 - eta * eta * (1.0 - NdotI * NdotI);
    if(k < 0.0)
        return vec3(0.0);
    else
        return eta * I - (eta * NdotI + sqrt(k)) * N;
}


void main() {

	vec3 norm = normalize(fs_in.fragNormal);

    float theta = 0.0f;
    float phi = 0.0f;

	vec3 V = fs_in.Pos;
    
    if(isNormMapping)
        V = fs_in.Norm;

    vec2 texCoords = CalcCubeMap(V);
     
    switch (uvType) {
        case 0:
            texCoords = fs_in.TexCoords;
            break;
		case 1:
            texCoords = CalcCubeMap(V);
            break;

		case 2:
            theta = degrees(atan(V.y, V.x));
            theta += 180.0f;
            texCoords.x = theta / 360.0f;

            float zdiff = ((zMax - zMin) == 0.0f) ? 1.0f : zMax - zMin;

            texCoords.y = (V.z - zMin) / (zdiff);
            break;

		case 3:
            theta = degrees(atan(V.y, V.x));
            theta += 180.0f;

            phi = degrees(acos(V.z / length(V)));

            texCoords.x = theta / 360.0f;
            texCoords.y = (phi / 180.0f);

            break;
        default:
            texCoords = fs_in.TexCoords;
    }


    vec3 viewDir = normalize(camera - fs_in.fragPos);

    vec3 result = vec3(0.0, 0.0, 0.0);

    for(int i = 0; i < numOfLights; ++i)
    {
        if(light[i].type == 1)
            result += CalcSpotLight(light[i], norm, viewDir, texCoords);
        else if(light[i].type == 2)
            result += CalcDirLight(light[i], norm, viewDir, texCoords);
        else
            result += CalcPointLight(light[i], norm, viewDir, texCoords);
    }
 
    vec3 Kd = texture(diffuse_texture, texCoords).xyz;
    result += I_emissive + global_ambient * Kd;

    float dist = length(camera - fs_in.fragPos);
    float fogFactor = (far - dist) / (far - near);

    fogFactor = clamp(fogFactor, 0.0, 1.0);

    result = mix(fog_color, result, fogFactor);
    result = clamp(result, 0, 1);

    // TODO: Move camera pos out of light uniform block and calc these on vert shader
    // TODO: pass viewDir, reflect, refract vector as an vs_out

    fragColor = vec4(result, 1.0);
}
