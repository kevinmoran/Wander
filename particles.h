#pragma once

#include "maths_funcs.h"

#define NUM_PARTICLES 32

vec2 particle_origin;
vec2 particle_pos[NUM_PARTICLES];
vec2 particle_vels[NUM_PARTICLES];
vec2 particle_sizes[NUM_PARTICLES];
vec4 particle_cols[NUM_PARTICLES];
float particle_angles[NUM_PARTICLES]; 
float particle_lifetimes[NUM_PARTICLES];

//Hypothetical usage code
/*
-----GAME CODE-----
if(collision){
	ParticleSystem::add_explosion(vec2 origin);
	ParticleSystem::add_explosion(*parent);
	ParticleSystem::add_explosion(parent_handle);
}

-----BEHIND THE SCENES-----
ParticleSystem::update(double dt){
	//Update all active particle sources
	//If source is expired, remove from list, free memory
}

PARTICLE_SOURCE_HANDLE ParticleSystem::create_particles(vec2 origin){
	//Get some free memory for a particle sources
	//Find free slot in memory pool
	//Or dynamically allocate new source
	//mark slot as occupied
	//add source to update list
	return handle to source

	PARTICLE_SOURCE_HANDLE new_handle;
	//Decide what to do if we can't get one!

	return new_handle;
}

ParticleSystem::add_explosion(vec2 origin){
	handle = ParticleSystem::create_particles(origin);
	ParticleSystem::particle_sources[handle].duration = EXPLOSION_DURATION;
	ParticleSystem::particle_sources[handle].num_live_particles = EXPLOSION_NUM_PARTICLES_INITIAL;

	for all particles in particle_source[handle]{
		vel = EXPLOSION_VEL_RANGE;
		pos = origin;
		size = EXPLOSION_SIZE_RANGE;
		colour = EXPLOSION_COLOUR_RANGE;
		lifetime = EXPLOSION_LIFETIME_RANGE;
	}
}

*/

//TODO NONE OF THIS IS FINISHED
//DECIDING ON A DECENT PARTICLE SYSTEM IMPLEMENTATION
struct Particle {
	vec2 vel;
	//float ang_vel;
	vec2 pos;
	//float angle;
	vec2 size;
	vec4 colour;
	float lifetime;
};

//Think there should be a difference between a "fire-and-forget" particle sources
//e.g. an explosion
//vs a persistent effect like a smoke source

struct ParticleSource {
	//bool is_alive;
	vec2 origin;
	int num_live_particles;
	//double emission_timer;
	//double emission_period;
	Particle particles[NUM_PARTICLES];
};

//How to make this dynamic?
//Just pre-allocate a ton of sources (wasteful, and if e.g. sources[1] expires we have a gap)
//Dynamic array (memory fragmentation)
struct ParticleSystem {
	ParticleSource sources[8];
	int num_sources_active;
};

//NB What do we do if the source is moving?
/*
struct MovingParticleSource{
	vec2* parent_position;
	//or
	Entity* parent; //might be handy to base velocity on parent's!
}
*/

void update_particles(ParticleSource &source, double dt){
	for(int i=0; i<source.num_live_particles; i++){
		source.particles[i].lifetime -= dt;
		if(source.particles[i].lifetime<0){
			//Particle has died, fill this hole in the array
			//Move last live particle in array to this slot
			source.particles[i] = source.particles[source.num_live_particles-1];
			source.num_live_particles--;
			if(!source.num_live_particles) return; //out of particles
		}

		//Update particle i
		source.particles[i].vel += vec2(0, -9.81f);
		source.particles[i].pos += source.particles[i].vel;
		//source.particles[i].angle += source.particles[i].ang_vel;
		source.particles[i].colour.v[3] = MIN(source.particles[i].lifetime, 1);
	}
}

//Copies contents of p2 into p1 and frees p2
//To move last particle in array to hole created when another dies
// void move_particle(Particle &p1, Particle &p2){
// }
