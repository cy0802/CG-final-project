#version 330 core

out vec4 FragColor;
in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform sampler2D texture_;

void main(){
	FragColor = texture(texture_, texCoord) * vec4(1.0, 1.0, 1.0, 1.0);
}