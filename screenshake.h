#pragma once
#include "maths_funcs.h"

float screenshake_timer = 0.0f;
const float SCREENSHAKE_DURATION = 0.08f;
bool screen_is_shaking = false;

inline float screenshake(double shake_time, double duration, float A, float f){
	double t = (duration-shake_time)/duration;
	return A*t*sin(2*M_PI*f*t);
}
