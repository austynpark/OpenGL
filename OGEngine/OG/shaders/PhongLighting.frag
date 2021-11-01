#version 450 core

out vec4 fragColor;

in vec3 result;

void main() {
    fragColor = vec4(result, 1.0);
}
