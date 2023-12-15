#version 330 core
layout (location = 0) in vec3 aPos;

// out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 local;

void main(){
	gl_Position = projection * view * model * local * vec4(aPos, 1.0);
	// gl_Position = vec4(aPos, 1.0);
}