#pragma once
#include "Entity.h"

struct AABB {
	float left, right, top, bottom;

	AABB() {}
	AABB(float l, float r, float t, float b) {
		left = l;
		right = r;
		top = t;
		bottom = b;
	};
};

// AABB generateAABB(float x, float y, float width, float height){
	
// }

AABB generateAABB(const Entity &ent){
	float left = ent.x - ent.width/2;
	float right = ent.x + ent.width/2;
	float top = ent.y + ent.height/2;
	float bottom = ent.y - ent.height/2;
	return AABB(left, right, top, bottom);
}

bool checkCollision(const AABB &box1, const AABB &box2){
	return (box1.right > box2.left 
		&& box1.left < box2.right 
		&& box1.bottom<box2.top 
		&& box1.top>box2.bottom);
}
