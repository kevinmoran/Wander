#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "time.h"

#include "init_gl.h"
#include "maths_funcs.h"
#include "input.h"
#include "Shader.h"
#include "Entity.h"
#include "Player.h"
#include "collision2D.h"
#include "Cam2D.h"
#include "screenshake.h"
#include "rand_utils.h"
#include "particles.h"

GLFWwindow* window = NULL;
int gl_width = 360;
int gl_height = 240;
float gl_aspect_ratio = (float)gl_width/gl_height;

int main() {
	if (!init_gl(window, gl_width, gl_height)){ return 1; }

	//Geometry setup
	GLfloat quad_points[] = {
		-0.5f,	0.5f,
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		-0.5f,	0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f
	};

	GLuint quad_vao;
	GLuint quad_points_vbo;
	glGenBuffers(1, &quad_points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quad_points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_points), quad_points, GL_STATIC_DRAW);

	glGenVertexArrays(1, &quad_vao);
	glBindVertexArray(quad_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_points_vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	//Load shader
	Shader pass_shader("pass.vert", "pass.frag");
	GLuint colour_loc = glGetUniformLocation(pass_shader.prog_id, "quad_colour");

	//Setup objects in scene
	const int NUM_BLOCKS = 2;
	const int NUM_ENEMIES = 2;
	Entity blocks[NUM_BLOCKS];
	Entity enemies[NUM_ENEMIES];
	AABB blocksAABBs[NUM_BLOCKS];
	AABB enemiesAABBs[NUM_ENEMIES];

	//TODO: blue noise distribution?
	srand((unsigned int)time(0));
	for(int i=0; i<NUM_BLOCKS; i++){
		blocks[i].x = rand_betweenf(-1,1);
		blocks[i].y = rand_betweenf(-1,1);
		blocks[i].colour = vec4(rand_betweenf(0.5f,1), rand_betweenf(0.7f,1), rand_betweenf(0.8f,1), 1);
		blocks[i].set_model_matrix();
		//recalculate collision volume
		blocksAABBs[i] = generateAABB(blocks[i]);
	}

	for(int i=0; i<NUM_ENEMIES; i++){
		enemies[i].x = rand_betweenf(-1,1);
		enemies[i].y = rand_betweenf(-1,1);
		enemies[i].colour = vec4(0, rand_betweenf(0.6f,1), rand_betweenf(0,0.25f), 1);
		enemies[i].set_model_matrix();
		//recalculate collision volume
		enemiesAABBs[i] = generateAABB(enemies[i]);
	}

	//Camera setup
	Cam2D camera(0, 0, gl_aspect_ratio);
	glUseProgram(pass_shader.prog_id);
	glUniformMatrix4fv(pass_shader.V_loc, 1, GL_FALSE, camera.V.m);
	glUniformMatrix4fv(pass_shader.P_loc, 1, GL_FALSE, camera.P.m);

	//Timers and other assorted variables
	float particle_timer = 0.0f;
	const float PARTICLES_DURATION = 0.8f;
	bool is_drawing_particles = false;

	double curr_time = glfwGetTime(), prev_time, dt;
	//-------------------------------------------------------------------------------------//
	//-------------------------------------MAIN LOOP---------------------------------------//
	//-------------------------------------------------------------------------------------//
	while (!glfwWindowShouldClose(window)) {
		//Get dt
		prev_time = curr_time;
		curr_time = glfwGetTime();
		dt = curr_time - prev_time;
		if (dt > 0.1) dt = 0.1;

		screenshake_timer+=dt;
		particle_timer+=dt;

		// static float fps_timer = 0.0f;
		// fps_timer+=dt;
		// if(fps_timer>0.2f){
		// 	char title_string[64];
		// 	sprintf(title_string, "Wander - %.2fHz", 1/dt);
		// 	glfwSetWindowTitle(window, title_string);
		// 	fps_timer = 0.0f;
		// }

		//Get Input
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
			continue;
		}

		//Update game state

		update_player(dt);

		//Move enemies
		for(int i=0; i<NUM_ENEMIES; i++){
			enemies[i].vel = enemies[i].vel*friction_factor;
			enemies[i].x += enemies[i].vel.v[0]*dt;
			enemies[i].y += enemies[i].vel.v[1]*dt;
			enemies[i].set_model_matrix();
			enemiesAABBs[i] = generateAABB(enemies[i]);
		}

		//Check collisions with blocks
		playerAABB = generateAABB(player);

		for(int i=0; i<NUM_BLOCKS; i++){
			if(checkCollision(playerAABB, blocksAABBs[i])){
				player.x -= player.vel.v[0]*dt;
				player.y -= player.vel.v[1]*dt;
				player.vel *= player_restitution;
			}
		}

		//Check collisions with enemies
		for(int i=0; i<NUM_ENEMIES; i++){
			if(checkCollision(playerAABB, enemiesAABBs[i])){
				enemies[i].vel = player.vel*1.5f;

				//Spawn particles
				is_drawing_particles = true;
				particle_timer = 0.0f;
				particle_origin = vec2(enemies[i].x, enemies[i].y);
				for(int i=0; i<NUM_PARTICLES; ++i){
					particle_pos[i] = particle_origin;
					particle_vels[i] = vec2(rand_betweenf(-1,1), rand_betweenf(-1,1));
					particle_cols[i] = vec4(rand_betweenf(0.7f,1), 0.2f, rand_betweenf(0.2f, 0.7f), 1);
					particle_angles[i] = rand_betweenf(0,360);
					particle_lifetimes[i] = PARTICLES_DURATION;
				}
			}
		}

		//Screenshake
		if(screenshake_timer>SCREENSHAKE_DURATION) {
			screen_is_shaking = false;
		}
		if(screen_is_shaking){
			float shake_val = screenshake(screenshake_timer, SCREENSHAKE_DURATION, 0.08f, 2);
			camera.x += shake_val;
			camera.set_view_matrix();
			glUniformMatrix4fv(pass_shader.V_loc, 1, GL_FALSE, camera.V.m);
			camera.x -= shake_val;
		}

		//Particles
		if(particle_timer>PARTICLES_DURATION){
			is_drawing_particles = false;
		}
		if(is_drawing_particles){
			for(int i=0; i<NUM_PARTICLES; ++i){
				particle_pos[i] += particle_vels[i]*dt;
			}
		}

		//Rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(pass_shader.prog_id);
		glBindVertexArray(quad_vao);

		//Draw player
    	player.set_model_matrix();
		glUniformMatrix4fv(pass_shader.M_loc, 1, GL_FALSE, player.M.m);
		glUniform4fv(colour_loc, 1, player.colour.v);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//Draw blocks
		for(int i=0; i<NUM_BLOCKS; i++){
			glUniformMatrix4fv(pass_shader.M_loc, 1, GL_FALSE, blocks[i].M.m);
			glUniform4fv(colour_loc, 1, blocks[i].colour.v);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		//Draw enemies
		for(int i=0; i<NUM_ENEMIES; i++){
			glUniformMatrix4fv(pass_shader.M_loc, 1, GL_FALSE, enemies[i].M.m);
			glUniform4fv(colour_loc, 1, enemies[i].colour.v);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		//Draw particles
		if(is_drawing_particles){
			for(int i=0; i<NUM_PARTICLES; ++i){
				mat4 particle_M = identity_mat4();
				particle_M = scale(particle_M, 0.1f);
				particle_M = translate(particle_M, vec3(particle_pos[i], 0));
				glUniformMatrix4fv(pass_shader.M_loc, 1, GL_FALSE, particle_M.m);
				glUniform4fv(colour_loc, 1, particle_cols[i].v);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}

		glfwSwapBuffers(window);

	}//end main loop

	return 0;
}
