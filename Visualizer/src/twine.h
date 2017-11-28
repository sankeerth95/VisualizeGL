#pragma once
#define NUM_POINTS 200


void initTwine(float * initstate);
void twineSim();
float getState(int i);
void updateTwineRenderGL(unsigned int buffer);

//void updateTwineRenderGL(unsigned int buffer) {


