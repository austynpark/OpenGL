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

        vec2 uv = vec2(1.0);

        // POSITIVE & NEGATIVE X
        if ((absX >= absY) && (absX >= absZ))
        {
			uv.y += y / absX;

            if (x > 0)
            {
                uv.x += z / absX;
                return texture(right, uv / 2.0f);
                // Right
            }
            else
            {
                uv.x += z / absX;
                return texture(left, uv / 2.0f);
                // Left
            }
        }
        // POSITIVE & NEGATIVE Y
        else if ((absY >= absX) && (absY >= absZ))
        {
            uv.x += x / absY;

            if (y > 0)
            {
                uv.y += z / absY;
                return texture(top, uv / 2.0f);
                // Top
            }
            else
            {
                uv.y += z / absY;
                return texture(bottom, uv / 2.0f);
                // Bottom
            }
        }
        // POSITIVE & NEGATIVE Z
        else if (absZ >= absX && absZ >= absY)
        {
			uv.y += y / absZ;

            if (z > 0)
            {
                uv.x += x / absZ;
                return texture(front, uv / 2.0f);
                // Foward
            }
            else
            {
                uv.x += x / absZ;
                return texture(back, uv / 2.0f);
                // Back
            }
        }

        return vec4(1.0f);
}

void main()
{
	fragColor = CalcCubeMap(TexCoords);
}
