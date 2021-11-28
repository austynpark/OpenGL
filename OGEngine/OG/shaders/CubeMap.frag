#version 450 core

layout(location = 0) in vec3 TexCoords;

out vec4 fragColor;

uniform sampler2D right;
uniform sampler2D left;
uniform sampler2D top;
uniform sampler2D bottom;
uniform sampler2D front;
uniform sampler2D back;

vec4 CalcCubeMap(vec3 vEntity)
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
                return texture(right, (uv + vec2(1.0)) * 0.5f);
            }
            else
            {
                uv.x = z / absX;
                return texture(left, (uv + vec2(1.0)) * 0.5f);
            }
        }
        // POSITIVE & NEGATIVE Y
        else if ((absY >= absX) && (absY >= absZ))
        {
            uv.x = x / absY;

            if (y > 0)
            {
                uv.y = -z / absY;
                return texture(top, (uv + vec2(1.0)) * 0.5f);
            }
            else
            {
                uv.y = z / absY;
                return texture(bottom, (uv + vec2(1.0)) * 0.5f);
            }
        }
        // POSITIVE & NEGATIVE Z
        else if (absZ >= absX && absZ >= absY)
        {
			uv.y = y / absZ;

            if (z > 0)
            {
                uv.x = x / absZ;
                return texture(front, (uv + vec2(1.0)) * 0.5f);
            }
            else
            {
                uv.x = -x / absZ;
                return texture(back, (uv + vec2(1.0)) * 0.5f);
            }
        }

        // Convert range from -1 to 1 to 0 to 1
		return vec4(0.0f);
}

void main()
{
	fragColor = CalcCubeMap(TexCoords);
}
