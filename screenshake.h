#include "maths_funcs.h"

inline float screenshake(double shake_time, double duration, float A, float f){
	double t = (duration-shake_time)/duration;
	return A*sin(2*M_PI*f*t);
}
