#ifndef RENDER_H 
#define RENDER_H

#include "basic.h"

struct texture {
	s32 width, height;
	u32 id;
};


texture MakeTexture (s32 width, s32 height, u8_array pixels);

const f32 DEFAULT_X_ALIGNMENT = 0.5f;
const f32 DEFAULT_Y_ALIGNMENT = 0.0f;
const f32 DEFAULT_FLIP = 0.0f;
void DrawTexturedRect(texture my_texture, f32 x, f32 y, f32 tx, f32 ty, f32 width, f32 height, f32 x_alignment = DEFAULT_X_ALIGNMENT, f32 y_alignment = DEFAULT_Y_ALIGNMENT, f32 x_flip = DEFAULT_FLIP, f32 y_flip = DEFAULT_FLIP);

#endif