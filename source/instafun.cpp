#ifndef INSTAFUN_H 
#define INSTAFUN_H

#include "basic.h"
#include "bmp.cpp"
#include "render.cpp"
#include "win32_instafun.cpp"

struct State {
	win32_window window;
	texture sprites;
	texture background;
};

extern "C" __declspec(dllexport) INIT_SIGNATURE(Init){
	auto state = new State;  
	state->window;
	Win32CreateWindow(&state->window, api, 640, 480, "InstaFun");

	state->sprites = LoadBMPTexture("data/sprites.bmp");
	state->background = LoadBMPTexture("data/stage.bmp");

	return (u8 *) state;
}

extern "C" __declspec(dllexport) UPDATE_SIGNATURE(Update){
	auto state = (State *) init_data;

	glViewport(0, 0, state->window.width, state->window.height);
  	glClearColor(0, 0.5f, 0.5f, 1.0f);
  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL, 1/255.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, state->background.id);

    glBegin(GL_TRIANGLES);
      glTexCoord2f(0.01f, 0);
      glVertex2f(-1, -1);

      glTexCoord2f(1, 0);
      glVertex2f(1, -1);
      
      glTexCoord2f(1, 1);
      glVertex2f(1, 1);

      glTexCoord2f(0.01f, 1);
      glVertex2f(-1, 1);

      glTexCoord2f(0.01f, 0);
      glVertex2f(-1, -1);

      glTexCoord2f(1, 1);
      glVertex2f(1, 1);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, state->sprites.id);

  	glBegin(GL_TRIANGLES);
      glTexCoord2f(0, 0);
    	glVertex2f(-0.5f, -0.5f);

      glTexCoord2f(1, 0);
    	glVertex2f(0.5f, -0.5f);
    	
      glTexCoord2f(1, 1);
      glVertex2f(0.5f, 0.5f);

      glTexCoord2f(0, 1);
      glVertex2f(-0.5f, 0.5f);

      glTexCoord2f(0, 0);
      glVertex2f(-0.5f, -0.5f);

      glTexCoord2f(1, 1);
      glVertex2f(0.5f, 0.5f);
  	glEnd();
      
  	SwapBuffers(state->window.device_context);
}
#endif