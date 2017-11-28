#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/rotate_vector.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <math.h>

#include "twine.h"

std::string getSource(const std::string& file) {

	std::ifstream fd(file);
	std::string s;
	std::string src = "";
	while (std::getline(fd, s))
		src += s + "\n";

	fd.close();
	return src;
}

unsigned int createShader(unsigned int type, const std::string& source) {
	unsigned int shader = glCreateShader(type);
	const char* src = source.c_str();

	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char *message = (char *)alloca(sizeof(char)*length);
		glGetShaderInfoLog(shader, length, &length, message);
		return status;
	}

	return shader;
}

unsigned int program;
unsigned int uni_location;
std::string shaderPath = "c:\\dev\\attempts\\Visualizer\\Visualizer\\src\\";

typedef struct singleBuf {
	unsigned int vao;
	unsigned int vbo;
} singleBuffer;
typedef struct drawable1Buffer {
	int numPoints;
	singleBuffer vo;
} drawable1Buffer;

void initShaders() {
	program = glCreateProgram();
	unsigned int vs = createShader(GL_VERTEX_SHADER, getSource(shaderPath + "vertex.shader") );
	unsigned int fs = createShader(GL_FRAGMENT_SHADER, getSource(shaderPath + "fragment.shader") );

	if (vs == GL_FALSE || fs == GL_FALSE) {
		exit(12);
	}
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	glValidateProgram(program);
	int result;
	glGetProgramiv(program, GL_LINK_STATUS, &result);

	if (result == GL_FALSE) {
		int length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		char *message = (char *)alloca(sizeof(char)*length);
		glGetShaderInfoLog(program, length, &length, message);
		exit(2);
	}
	glUseProgram(program);

	uni_location = glGetUniformLocation(program, "u_T");
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), 30.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(uni_location, 1, GL_FALSE, (const GLfloat*)&R);
}

singleBuffer setupBuffer(float *gridData, int numPoints) {
	unsigned int gridVao;
	glGenVertexArrays(1, &gridVao);
	glBindVertexArray(gridVao);

	unsigned int buffer[1];
	glGenBuffers(1, buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * numPoints*sizeof(float), gridData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void *)(sizeof(float)*3u) );

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return { gridVao, buffer[0] };
}

drawable1Buffer setupGrid() {

	float x0 = -1.0f;
	int gridPoints = 40; float gridIntensity = 0.2f;
	float *gridData = new float[6*gridPoints];

	for (int i = 0; i < 6*gridPoints/2; i += 12) {
		gridData[i] = x0; gridData[i + 1] = -1.0f; gridData[i + 2] = 0.0f;
		gridData[i+3] = gridIntensity; gridData[i + 4] = gridIntensity; gridData[i + 5] = gridIntensity;

		gridData[i + 6] = x0; gridData[i + 7] = 1.0f; gridData[i + 8] = 0.0f;
		gridData[i + 9] = gridIntensity; gridData[i + 10] = gridIntensity; gridData[i + 11] = gridIntensity;

		x0 += 0.2f;
	}
	x0 = -1.0f;
	for (int i = 6*gridPoints/2; i < 6*gridPoints; i += 12) {
		gridData[i] = -1.0f; gridData[i + 1] = x0; gridData[i + 2] = 0.0f;
		gridData[i + 3] = gridIntensity; gridData[i + 4] = gridIntensity; gridData[i + 5] = gridIntensity;

		gridData[i + 6] = 1.0f; gridData[i + 7] = x0; gridData[i + 8] = 0.0f;
		gridData[i + 9] = gridIntensity; gridData[i + 10] = gridIntensity; gridData[i + 11] = gridIntensity;

		x0 += 0.2f;
	}

	int i = 6 * 10;
	gridData[i + 3] = 1.0f; gridData[i + 4] = 1.0f; gridData[i + 5] = 1.0f;
	gridData[i + 9] = 1.0f; gridData[i + 10] = 1.0f; gridData[i + 11] = 1.0f;

	i = 6 * gridPoints / 2 + 6 * 10;
	gridData[i + 3] = 1.0f; gridData[i + 4] = 1.0f; gridData[i + 5] = 1.0f;
	gridData[i + 9] = 1.0f; gridData[i + 10] = 1.0f; gridData[i + 11] = 1.0f;

	drawable1Buffer ret = { gridPoints, setupBuffer(gridData, gridPoints) };
	delete[] gridData;

	return ret;
}

drawable1Buffer staticPlot(float *datax, float *datay, int size) {

	float *gridData = new float[6 * size];
	float rIntensity = 0.0f, gIntensity = 1.0f, bIntensity = 0.0f;
	for (int i = 0; i < 6 * size; i += 6) {
		gridData[i] = *datax; gridData[i + 1] = *datay;
		datax++; datay++;
		gridData[i + 3] = rIntensity; gridData[i + 4] = gIntensity; gridData[i + 5] = bIntensity;
	}

	drawable1Buffer ret = { size, setupBuffer(gridData, size) };
	delete[] gridData;
	return ret;
}

//delaunay? nope, just plain framed
//requirements, 
drawable1Buffer staticSurfacePlot(float *datax, float *datay, float *dataz, int size) {

	float *gridData = new float[6 * size];
	float rIntensity = 0.0f, gIntensity = 1.0f, bIntensity = 0.0f;
	for (int i = 0; i < 6 * size; i += 6) {
		gridData[i] = *datax; gridData[i + 1] = *datay;
		datax++; datay++;
		gridData[i + 3] = rIntensity; gridData[i + 4] = gIntensity; gridData[i + 5] = bIntensity;
	}

	drawable1Buffer ret = { size, setupBuffer(gridData, size) };
	delete[] gridData;
	return ret;
}

void wasteCycles(unsigned int n) {
	for (unsigned int i = 0; i < n; i++);
}

int main() {

	if(!glfwInit())
		return -1;

	GLFWwindow *window = glfwCreateWindow(640, 480, "Visuals", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwShowWindow(window);

	if (glewInit() != GLEW_OK)
		exit(1);

	initShaders();
	drawable1Buffer grid = setupGrid();
	
	float datay[NUM_POINTS], datax[NUM_POINTS];
	for (int i = 0; i < NUM_POINTS; i += 1) {
		datax[i] = -1.0f + 0.01f*i;
		datay[i] = sin(4.7f * datax[i])*sin(4.7f*datax[i]);
		if (i > NUM_POINTS / 2)
			datay[i] = 0.0f;
	}
	initTwine(datay);
	drawable1Buffer plot = staticPlot(datax, datay, NUM_POINTS);
	
	while (!glfwWindowShouldClose(window)) {
		
		twineSim();
		updateTwineRenderGL(plot.vo.vbo);
		wasteCycles(10000000);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(grid.vo.vao);
		glDrawArrays(GL_LINES, 0, grid.numPoints);

		glBindVertexArray(plot.vo.vao);
		glDrawArrays(GL_LINE_STRIP, 0, plot.numPoints);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	glfwTerminate();
	return 0;
}

