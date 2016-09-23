#pragma once
//#include <GL/glew.h>
#include "maths_funcs.h"

//This should definitely be SOA
//vel and pos are iterated over for update
//M and colour for render
struct Entity {
	vec2 vel;
	float x, y, z;
	mat4 M;
	vec4 colour;
	float width, height;
	float angle;

	Entity();
	void set_model_matrix();
};

void Entity::set_model_matrix() {
	mat4 temp = identity_mat4();
	temp = scale(temp, vec3(width, height, 1));
	temp = rotate_z_deg(temp, angle);
	temp = translate(temp, vec3(x,y,z));
	M = temp;
}

Entity::Entity() {
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	width = 0.2f;
	height = 0.2f;
	colour = vec4(0.8f, 0.0f, 0.0f, 1.0f);
	angle = 0.0f;
	vel = vec2(0.0f, 0.0f);
	set_model_matrix();
}

