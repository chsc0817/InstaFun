#pragma once

#include "basic.h"
#include "win32_platform.h"
#include "bmp.cpp"
#include "render.cpp"
#include "input.cpp"
#include "random.cpp"

const f32 target_width_over_height = 16.0f/9.0f;

enum game_mode {
    GAME_MODE_MENU, 
    GAME_MODE_PLAY,
    GAME_MODE_DONE
};

struct player {
    f32 x,y;
    f32 x_flip;
    bool is_ready;
    bool has_won;
};

struct game_state {
	win32_window window;
    game_mode mode;
    player players[2];
    f32 showdown_countdown;
    random_generator rng;
	texture sprites;
	texture background;
    texture ready_text;
    texture ok_text;
    texture go_text;
};

texture LoadBMPTextureFromFile(win32_api *api, cstring file_path) {
    auto result = api->read_entire_file(file_path);
    assert(result.ok);
    texture my_texture = LoadBMPTexture(result.data);
    api->free_file_data(result.data);
    
    return my_texture;
}

INIT_DECLARATION {
	auto state = new game_state;
    *state = {};

    SYSTEMTIME system_time;
    GetSystemTime(&system_time);    
    FILETIME file_time;
    SystemTimeToFileTime(&system_time, &file_time);

    state->rng = MakeRandomGenerator(file_time.dwLowDateTime + file_time.dwHighDateTime);

	state->window;
	api->create_window(&state->window, api, 1280 * target_width_over_height, 1280, "InstaFun");

    state->mode = GAME_MODE_MENU;

    state->players[0] = {-128, -100, 0};
    state->players[1] = { 128, -120, 1};
	
    state->sprites = LoadBMPTextureFromFile(api, "data/sprites.bmp");
	state->background = LoadBMPTextureFromFile(api, "data/stage.bmp");
    state->ready_text = LoadBMPTextureFromFile(api, "data/ready.bmp");
    state->ok_text = LoadBMPTextureFromFile(api, "data/ok.bmp");
    state->go_text = LoadBMPTextureFromFile(api, "data/go.bmp");

	return (u8 *) state;
}

UPDATE_DECLARATION {
	auto state = (game_state *) init_data;
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
    glDisable(GL_DEPTH_TEST);

    DrawTexture(state->background, 0, 0, 0.5f, 0.5f);

    u32 player_input[2] = {'A', 'L'};

    switch(state->mode) {
        case GAME_MODE_MENU: {
            DrawTexture(state->ready_text, 0, 70);

            bool everyone_ready = true;

            for (u32 i = 0; i < ArrayCountOf(state->players); i++) {
                if (WasPressed(api->input.keys[player_input[i]])) {
                    state->players[i].is_ready = true;
                }
                if (!state->players[i].is_ready) {
                    everyone_ready = false;
                } else {
                    DrawTexture(state->ok_text, state->players[i].x, state->players[i].y + 60);
                }
            }

            if (everyone_ready) {
                state->mode = GAME_MODE_PLAY;
                state->showdown_countdown = RandomF32(&state->rng, 2, 6.1f);
                printf("%f \n", state->showdown_countdown);
            }
        } break;
        case GAME_MODE_PLAY: {
            state->showdown_countdown -= api->delta_seconds;

            if(state->showdown_countdown <= 0 ) {
                DrawTexture(state->go_text, 0, 10);
            }

            bool invert_win = (state->showdown_countdown > 0);
            bool is_done = false;

            for (u32 i = 0; i < ArrayCountOf(state->players); i++) {

                if (WasPressed(api->input.keys[player_input[i]])) {
                    state->players[i].has_won = true;
                    is_done = true;
                }
            }
            if (is_done) {
                for (u32 i = 0; i < ArrayCountOf(state->players); i++) {
                    state->players[i].has_won ^= invert_win;
                }
                state->mode = GAME_MODE_DONE;
            }
        } break;

        case GAME_MODE_DONE: {
            for (u32 i = 0; i < ArrayCountOf(state->players); i++) {
                if (state->players[i].has_won) 
                    continue;

                f32 center_x = 0;
                f32 center_y = -110;

                f32 x = Lerp(state->players[i].x, center_x, 0.8f);
                f32 y = Lerp(state->players[i].y, center_y, 0.8f);

                DrawTexturedRect(state->sprites, x, y, 0, state->sprites.height - 123, 64, 64, DEFAULT_X_ALIGNMENT, DEFAULT_Y_ALIGNMENT, state->players[i].x_flip, DEFAULT_FLIP);     
            }

            for (u32 i = 0; i < ArrayCountOf(state->players); i++) {
                if (!state->players[i].has_won) 
                    continue;

                f32 center_x = 0;
                f32 center_y = -110;

                f32 x = Lerp(state->players[i].x, center_x, 0.8f);
                f32 y = Lerp(state->players[i].y, center_y, 0.8f);

                DrawTexturedRect(state->sprites, x, y, 64, state->sprites.height - 63, 128, 64, 0.3f, DEFAULT_Y_ALIGNMENT, state->players[i].x_flip, DEFAULT_FLIP);     
                            
            }

            if (WasPressed(api->input.keys[' '])) {
                state->mode = GAME_MODE_MENU;
                for (u32 i = 0; i < ArrayCountOf(state->players); i++) {
                    state->players[i].is_ready = false;
                    state->players[i].has_won = false;
                }
            }

        } break;
    }
    if (state->mode != GAME_MODE_DONE) {
        for (u32 i = 0; i < ArrayCountOf(state->players); i++) {
            DrawTexturedRect(state->sprites, state->players[i].x, state->players[i].y, 0, state->sprites.height - 63, 64, 64, DEFAULT_X_ALIGNMENT, DEFAULT_Y_ALIGNMENT, state->players[i].x_flip, DEFAULT_FLIP);     
        }
    }
	
    api->display_window(&state->window);    
    
}
