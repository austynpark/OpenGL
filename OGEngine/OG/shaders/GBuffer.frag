#version 450 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gMaterial;
layout (location = 3) out vec3 gDepth;

in VS_OUT {
	vec3 fragPos;
	vec2 texCoords;
	vec3 fragNormal;
} fs_in;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;
uniform int uvType;
uniform float zMin;
uniform float zMax;
uniform bool isNormMapping;
uniform vec3 I_emissive;

vec2 CalcCubeMap(vec3 vEntity);

void main() {
	
	vec3 V = fs_in.fragPos;
	float theta = 0.0f;
    float phi = 0.0f;

    if(isNormMapping)
        V = fs_in.fragNormal;

    vec2 texCoords = CalcCubeMap(V);
     
    switch (uvType) {
        case 0:
            texCoords = fs_in.texCoords;
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
            texCoords = fs_in.texCoords;
    }

	gMaterial.rgb = texture(diffuse_texture, texCoords).rgb + I_emissive;
    gMaterial.a = texture(specular_texture, texCoords).r;

	gPosition = fs_in.fragPos;
	gNormal = normalize(fs_in.fragNormal);
    gDepth = vec3(length(fs_in.fragPos));
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

