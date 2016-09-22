#pragma once
#include "Entity.h"
#include "collision2D.h"
#include "input.h"
#include "screenshake.h"

Entity player;
AABB playerAABB = generateAABB(player);

//Player movement variables
float player_top_speed = 12.0f;
float player_acc = 16.0f;
float friction_factor = 0.85f; //higher is slippier
float player_restitution = -0.2f;

//Dash timer
float dash_timer = 0.0f;
const float DASH_DURATION = 0.1f;
bool is_dashing = false;

bool player_is_moving = false;

extern float gl_aspect_ratio;

void update_player(double dt){
    if (g_input[MOVE_UP]) {
        player.vel.v[1] += player_acc*dt;
        player_is_moving = true;
    }
    if (g_input[MOVE_LEFT]) {
        player.vel.v[0] -= player_acc*dt;
        player_is_moving = true;
    }
    if (g_input[MOVE_DOWN]) {
        player.vel.v[1] -= player_acc*dt;
        player_is_moving = true;
    }
    if (g_input[MOVE_RIGHT]) {
        player.vel.v[0] += player_acc*dt;
        player_is_moving = true;
    }
    if (g_input[DASH_MOVE] && length(player.vel)>player_top_speed*0.3f) {
        is_dashing = true;
        dash_timer = 0.0f;
    }
    
    //Clamp player speed
    if (length(player.vel) > player_top_speed) {
        player.vel = normalise(player.vel);
        player.vel *= player_top_speed;
    }
    //Check if dash has finished
    dash_timer += dt;
    if(dash_timer>DASH_DURATION){ 
        is_dashing = false;
    }
    //Dash logic
    if(is_dashing){
        float dash_speed = 6.0f*(DASH_DURATION-dash_timer)/DASH_DURATION;
        dash_speed = MAX(0, dash_speed);
        vec2 player_dir = normalise(player.vel);
        player.vel += player_dir*dash_speed;
    }

    //decelerate player
    if(!player_is_moving)
        player.vel = player.vel*friction_factor;

    //move player
    player.x += player.vel.v[0]*dt;
    player.y += player.vel.v[1]*dt;

    //Confine player to screen bounds
    if (player.x < -gl_aspect_ratio) {
        player.x = -gl_aspect_ratio;
        if(player.vel.v[0] <= -player_top_speed){
            screen_is_shaking = true;
            screenshake_timer = 0.0f;
        }
        player.vel.v[0] *= player_restitution;
    }
    else if (player.x > gl_aspect_ratio) {
        player.x = gl_aspect_ratio;
        if(player.vel.v[0] >= player_top_speed){
            screen_is_shaking = true;
            screenshake_timer = 0.0f;
        }
        player.vel.v[0] *= player_restitution;
    }
    if (player.y < -1.0f) {
        player.y = -1.0f;
        if(player.vel.v[1] <= -player_top_speed){
            screen_is_shaking = true;
            screenshake_timer = 0.0f;
        }
        player.vel.v[1] *= player_restitution;
    }
    else if (player.y > 1.0f) {
        player.y = 1.0f;
        if(player.vel.v[1] >= player_top_speed){
            screen_is_shaking = true;
            screenshake_timer = 0.0f;
        }
        player.vel.v[1] *= player_restitution;
    }
}
