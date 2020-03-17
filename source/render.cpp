#include <windows.h>
#include <gl/gl.h>
#include "render.h"

#pragma comment(lib, "opengl32.lib")

texture MakeTexture (s32 width, s32 height, u8_array pixels) {
	texture result;
	
	glGenTextures(1, &result.id);
	glBindTexture(GL_TEXTURE_2D, result.id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.base);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
	result.width = width;
	result.height = height;
    
	return result;
}