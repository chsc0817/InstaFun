#pragma once

#include "basic.h"
#include "win32_platform.h"
#include "bmp.cpp"
#include "render.cpp"
#include "input.cpp"

const f32 target_width_over_height = 16.0f/9.0f;

struct State {
	win32_window window;
	texture sprites;
	texture background;
};

texture LoadBMPTextureFromFile(win32_api *api, cstring file_path)
{
    auto result = api->read_entire_file(file_path);
    assert(result.ok);
    texture my_texture = LoadBMPTexture(result.data);
    api->free_file_data(result.data);
    
    return my_texture;
}

INIT_DECLARATION {
	auto state = new State;  
	state->window;
	api->create_window(&state->window, api, 1280 * target_width_over_height, 1280, "InstaFun");
	
    state->sprites = LoadBMPTextureFromFile(api, "data/sprites.bmp");
	state->background = LoadBMPTextureFromFile(api, "data/stage.bmp");
    
	return (u8 *) state;
}

UPDATE_DECLARATION {
	auto state = (State *) init_data;
	if ((state->window.width == 0) || (state->window.height == 0)) {
		return;
	}
    
	glDisable(GL_SCISSOR_TEST);
	glViewport(0, 0, state->window.width, state->window.height);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	f32 current_width_over_height = state->window.width / (f32) state->window.height;

	u32 canvas_width, canvas_height;

	if (current_width_over_height > target_width_over_height) {
		canvas_height = state->window.height;
		canvas_width = canvas_height * target_width_over_height;
	} else {
		canvas_width = state->window.width;
		canvas_height = canvas_width / target_width_over_height;
	}	
	glEnable(GL_SCISSOR_TEST);

	glViewport((state->window.width - canvas_width) * 0.5f, (state->window.height - canvas_height) * 0.5f, canvas_width, canvas_height);
	glScissor((state->window.width - canvas_width) * 0.5f, (state->window.height - canvas_height) * 0.5f, canvas_width, canvas_height);
    glClearColor(0, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL, 1/255.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#if 1
    glBindTexture(GL_TEXTURE_2D, state->background.id);
    
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0, 0);
    glVertex2f(-1.0f, -1.0f);
    
    glTexCoord2f(1, 0);
    glVertex2f(1.0f, -1.0f);
    
    glTexCoord2f(1, 1);
    glVertex2f(1.0f, 1.0f);
    
    glTexCoord2f(0, 1);
    glVertex2f(-1.0f, 1.0f);
    
    glTexCoord2f(0, 0);
    glVertex2f(-1.0f, -1.0f);
    
    glTexCoord2f(1, 1);
    glVertex2f(1.0f, 1.0f);
    glEnd();
#endif

    static f32 countdown;
    static u32 sprite_pose;
    const f32 pose_time = 2;
    const f32 delay_time = 0.5f;
	
    if(WasPressed(api->input.keys[' '])) {
		countdown = pose_time;
		sprite_pose = 1;
    } 

    if (countdown > 0) {
    	countdown -= api->delta_seconds;

    	if (countdown <= pose_time - delay_time) {
    		sprite_pose = 0;
    	}
	    if (countdown <= 0) {
	    	countdown = 0;
	    	sprite_pose = 1;
	    }
    }     

    if (sprite_pose % 2) {
    DrawTexturedRect(state->sprites, -128, -100, 0, state->sprites.height - 63, 64, 64);
	} else {
	DrawTexturedRect(state->sprites, -128, -100, 64, state->sprites.height - 63, 128, 64, 0.3f);	
	}

    DrawTexturedRect(state->sprites, 128, -120, 0, state->sprites.height - 63, 64, 64, DEFAULT_X_ALIGNMENT, DEFAULT_Y_ALIGNMENT, 1.0f);

    DrawTexturedRect(state->sprites, 128, 0, 64, state->sprites.height - 63, 128, 64, 0.3f, DEFAULT_Y_ALIGNMENT, 1.0f);
    
    api->display_window(&state->window);
    
}
