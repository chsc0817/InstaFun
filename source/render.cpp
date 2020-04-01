#include <windows.h>
#include <gl/gl.h>
#include "render.h"

#pragma comment(lib, "opengl32.lib")

f32 Lerp(f32 a, f32 b, f32 blend_factor) {
	f32 result = a * (1 - blend_factor) + b * blend_factor;
	return result;
}

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

void DrawTexturedRect(texture my_texture, f32 x, f32 y, f32 tx, f32 ty, f32 width, f32 height, f32 x_alignment, f32 y_alignment, f32 x_flip, f32 y_flip, f32 alpha) {
	glBindTexture(GL_TEXTURE_2D, my_texture.id);
	f32 texel_width = 1.0f / my_texture.width; 
    f32 texel_height = 1.0f / my_texture.height;

    const f32 canvas_texel_width = 1.0f / 256.0f;
    const f32 canvas_texel_height = canvas_texel_width * 16.0f / 9.0f;
    x_alignment = Lerp(x_alignment, 1 - x_alignment, x_flip);
    y_alignment = Lerp(y_alignment, 1 - y_alignment, y_flip);
    x -= x_alignment * width;
	y -= y_alignment * height;

	f32 tx_l = tx * texel_width;
	f32 tx_r = (tx + width) * texel_width;
	f32 tx0 = Lerp(tx_l, tx_r, x_flip);
	f32 tx1 = Lerp(tx_r, tx_l, x_flip);

	f32 ty_l = ty * texel_height;
	f32 ty_r = (ty + height) * texel_height;
	f32 ty0 = Lerp(ty_l, ty_r, y_flip);
	f32 ty1 = Lerp(ty_r, ty_l, y_flip);

    glBegin(GL_TRIANGLES);
    glTexCoord2f(tx0, ty0);
    glColor4f(1, 1, 1, alpha);
    glVertex2f(x * canvas_texel_width, y * canvas_texel_height);
    
    glTexCoord2f(tx1, ty0);
    glVertex2f((x + width) * canvas_texel_width, y * canvas_texel_height);
    
    glTexCoord2f(tx1, ty1);
    glVertex2f((x + width) * canvas_texel_width, (y + height) * canvas_texel_height);
    
    glTexCoord2f(tx0, ty1);
    glVertex2f(x * canvas_texel_width, (y + height) * canvas_texel_height);
    
    glTexCoord2f(tx0, ty0);
    glVertex2f(x * canvas_texel_width, y * canvas_texel_height);
    
    glTexCoord2f(tx1, ty1);
    glVertex2f((x + width) * canvas_texel_width, (y + height) * canvas_texel_height);
    glEnd();
}

void DrawTexture(texture my_texture, f32 x, f32 y, f32 x_alignment, f32 y_alignment, f32 x_flip, f32 y_flip, f32 alpha) {
	DrawTexturedRect(my_texture, x, y, 0, 0, my_texture.width, my_texture.height, x_alignment, y_alignment, x_flip, y_flip, alpha);
}