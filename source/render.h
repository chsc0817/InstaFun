#ifndef RENDER_H 
#define RENDER_H

#include "basic.h"

struct texture {
	s32 width, height;
	u32 id;
};


texture MakeTexture (s32 width, s32 height, u8_array pixels);


#endif