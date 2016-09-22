#pragma once
#include "maths_funcs.h"

#define DEFAULT_SCREEN_WIDTH 1080.0f
#define DEFAULT_SCREEN_HEIGHT 720.0f

struct Cam2D {
	float x, y;
	mat4 V;
	mat4 P;

	Cam2D();
	Cam2D(float x, float y, float aspect_ratio);

	void set_view_matrix();
};

Cam2D::Cam2D() {
	x = 0.0f;
	y = 0.0f;
	V = translate(identity_mat4(), vec3(x, y, -1.0f));
	P = orthographic(-DEFAULT_SCREEN_WIDTH/DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_WIDTH/DEFAULT_SCREEN_HEIGHT, 
						-1.0f, 1.0f, 
						0.1f, 100.0f);
}

Cam2D::Cam2D(float _x, float _y, float aspect_ratio) {
	x = _x;
	y = _y;
	V = translate(identity_mat4(), vec3(x, y, -1.0f));
	P = orthographic(-aspect_ratio, aspect_ratio, -1.0f, 1.0f, 0.1f, 100.0f);
}

//Recalculate V matrix. Should be called after changing x and/or y position
void Cam2D::set_view_matrix() {
	V = translate(identity_mat4(), vec3(x, y, -1.0f));
}

//Cam2D g_cam(0,0,360,240);
