#version 330

layout(location = 0) out vec4 color;
in vec3 colorIn;
void main() {
	color = vec4(colorIn, 1.0f);
}

