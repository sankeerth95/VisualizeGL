#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
out vec3 colorIn;
//layout(location = 1) in vec4 data;
//layout(location = 0) out vec4 color;
uniform mat4 u_T;

void main() {
	gl_Position = u_T*vec4(position, 0.8f);
	gl_Position.z = 0.0f;
	colorIn = color;
}
