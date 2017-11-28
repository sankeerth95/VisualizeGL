//#include <GLEW/glew.h>
#include "twine.h"
#include <GLEW/glew.h>

static float alpha1 = 0.5f;
static float prev[NUM_POINTS];
static float curr[NUM_POINTS];
static float state1[NUM_POINTS];

void initTwine(float *initstate) {

	for (int i = 0; i < NUM_POINTS;  i++) {
		state1[i] = initstate[i];
		curr[i] = initstate[i];
		prev[i] = initstate[i]/1.01f;
	}
}
void twineSim() {

	for (int i = 1; i < NUM_POINTS-1; i++) {
		state1[i] = -prev[i] + 2.0f*(1.0f - alpha1)*curr[i] + \
			alpha1*(curr[i + 1] + curr[i - 1]);
	}
	for (int i = 1; i < NUM_POINTS-1; i++) {
		prev[i] = curr[i];
		curr[i] = state1[i];
	}

}
float getState(int i){

	return state1[i];
}
void updateTwineRenderGL(unsigned int buffer) {

	float *gridData = new float[6 * NUM_POINTS];
	float rIntensity = 0.0f, gIntensity = 1.0f, bIntensity = 0.0f;

	int j = 0;
	for (int i = 0; i < 6 * NUM_POINTS; i += 6) {
		gridData[i] = -1.0f + j*2.0f / ((float)NUM_POINTS); gridData[i + 1] = getState(j);
		gridData[i + 2] = 0.0f;
		gridData[i + 3] = rIntensity; gridData[i + 4] = gIntensity; gridData[i + 5] = bIntensity;
		j++;
	}

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * NUM_POINTS*sizeof(float), gridData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] gridData;
}



