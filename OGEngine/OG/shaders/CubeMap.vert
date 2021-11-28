#version 450 core

layout(location = 0) in vec3  vPos;

layout(location = 0) out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	TexCoords = vPos;
	//gl_Position = (projection * view * vec4(vPos, 1.0)).xyww;
	gl_Position = projection * view * vec4(vPos, 1.0);
}
