#version 450 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec3 vNormal;

out vec3 result;

struct Light
{
    float inner_cone;
    float outer_cone;
    float fall_off;
	int type;
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    vec3 direction;
};

layout(std140) uniform Transforms
{
    mat4 view;
    mat4 projection;
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


uniform mat4 model;
uniform mat3 normalMatrix;
uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;
uniform int uvType;
uniform float zMin;
uniform float zMax;
uniform bool isNormMapping;

vec3 CalcDirLight(Light light, vec3 norm, vec3 viewDir, vec3 fragPos, vec2 texCoords) 
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 Ks = vec3(texture(specular_texture, texCoords));
    vec3 Kd = vec3(texture(diffuse_texture, texCoords));

    vec3 specular = vec3(0.0);

    if(diff > 0.0)
    {
		vec3 reflectDir = 2 * dot(norm, lightDir) * norm - lightDir;
        specular = Ks * pow(max(dot(viewDir, reflectDir), 0.0), Ks.r * Ks.r);
    }

	vec3 ambient = light.ambient * Kd;
    vec3 diffuse = light.diffuse * diff * Kd;
    specular *= light.specular;

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(Light light, vec3 norm, vec3 viewDir, vec3 fragPos, vec2 texCoords)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 Ks = vec3(texture(specular_texture, texCoords));
    vec3 Kd = vec3(texture(diffuse_texture, texCoords));

    vec3 specular = vec3(0.0);

    if(diff > 0.0)
    {
        vec3 reflectDir = reflect(-lightDir, norm);
        specular = Ks * pow(max(dot(viewDir, reflectDir), 0.0), Ks.r * Ks.r);
    }

    float dist = length(light.position - fragPos);
    float attenuation = min(1.0 / (att.x + att.y * dist + att.z * (dist * dist)), 1);

	vec3 ambient = light.ambient * Kd;
    vec3 diffuse = light.diffuse * diff * Kd;
    specular *= light.specular;

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(Light light, vec3 norm, vec3 viewDir, vec3 fragPos, vec2 texCoords)
{
    float theta = radians(light.inner_cone);
    float phi = radians(light.outer_cone);

    // spot light unit vector lightDir = (L), light.direction = (D)
    vec3 lightDir = normalize(light.position - fragPos);
    
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3 Ks = vec3(0);
    vec3 Kd = vec3(texture(diffuse_texture, texCoords));

    vec3 reflectDir = reflect(-lightDir, norm);

    // specular shading
    if(diff > 0)
    {
        Ks = vec3(texture(specular_texture, vUV)) * 
                pow(max(dot(viewDir, reflectDir), 0.0), Ks.r * Ks.r);
    }

    
    // attenuation
    float dist = length(light.position - fragPos);
    float attenuation = min(1.0 / (att.x + att.y * dist + att.z * (dist * dist)), 1);    
    
    float LdotD = max(dot(-lightDir, normalize(light.direction)), 0.0);
    float intesity = 0;

    if(LdotD > cos(theta))
    {
        intesity = 1;
    }
    else if(LdotD >= cos(phi))
    {
        intesity = pow(
            (LdotD - cos(phi)) / (cos(theta) - cos(phi))
            ,  light.fall_off);
    }

    vec3 diffuse = light.diffuse * diff * Kd;
    vec3 specular = light.specular * Ks;  
 
    diffuse *= attenuation * intesity;
    specular *= attenuation * intesity;
    return (diffuse + specular); 
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
			uv.y = y / absX;

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
                uv.y = -z / absY;
            else
                uv.y = z / absY;
        }
        // POSITIVE & NEGATIVE Z
        else if (absZ >= absX && absZ >= absY)
        {
			uv.y = y / absZ;

            if (z > 0)
                uv.x = x / absZ;
            else
                uv.x = -x / absZ;
        }

        // Convert range from -1 to 1 to 0 to 1
        return (uv + vec2(1.0)) * 0.5f;
}


void main() {
    vec3 norm = normalize(normalMatrix * vNormal);

    vec2 texCoords;
    
    float theta = 0.0f;
    float phi = 0.0f;

    vec3 V;
    if(isNormMapping)
        V = normalize(vNormal);
    else
        V = vPos;

    switch (uvType) {
        case 0:
            texCoords = vUV;
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
            texCoords = vUV;
    }



    vec3 fragPos = (model * vec4(vPos, 1.0)).xyz;

    vec3 viewDir = normalize(camera - fragPos);

    result = vec3(0.0, 0.0, 0.0);

    for(int i = 0; i < numOfLights; ++i)
    {
        if(light[i].type == 1)
            result += CalcSpotLight(light[i], norm, viewDir, fragPos, texCoords);
        else if(light[i].type == 2)
            result += CalcDirLight(light[i], norm, viewDir, fragPos, texCoords);
        else
            result += CalcPointLight(light[i], norm, viewDir, fragPos, texCoords);
    }

    result += global_ambient; 
    float dist = length(camera - fragPos);
    float fogFactor = (far - dist) / (far - near);

    fogFactor = clamp(fogFactor, 0.0, 1.0);

    result = mix(fog_color, result, fogFactor);

    gl_Position = projection * view * vec4(fragPos, 1.0);
}
