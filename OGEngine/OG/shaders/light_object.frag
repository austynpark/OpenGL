#version 450 core

out vec4 fragColor;
uniform vec3 diffuse;

void main() {
   fragColor = vec4(diffuse, 1.0);
}