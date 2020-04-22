#include <gl/gl.h>
#include <gl/glext.h>
#include "render.h"

#pragma comment(lib, "opengl32.lib")

#define GL_DEBUG_CALLBACK(name) void name (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)

typedef GL_DEBUG_CALLBACK((APIENTRY * DEBUGPROC));


typedef void (*glGenBuffers_function) (GLsizei n, GLuint * buffers);
typedef void (*glBindBuffer_function) (GLenum target, GLuint buffer);
typedef void (*glDeleteBuffers_function) (GLsizei n, const GLuint * buffers);
typedef void (*glBufferData_function) (GLenum target, GLsizeiptr size, const void * data, GLenum usage);
typedef void (*glVertexAttribPointer_function) (GLuint index,
											  	GLint size,
											  	GLenum type,
											  	GLboolean normalized,
											  	GLsizei stride,
											  	const void * pointer
											  	);
typedef void (*glEnableVertexAttribArray_function) (GLuint index);
typedef void (*glDisableVertexAttribArray_function) (GLuint index);
//typedef void (*glTexCoordPointer_function) (GLint size, GLenum type, GLsizei stride, const void * pointer);
//typedef void (*glColorPointer_function) (GLint size, GLenum type, GLsizei stride, const void * pointer);
typedef void (*glGenVertexArrays_function) (GLsizei n, GLuint *arrays);
typedef void (*glBindVertexArray_function)(GLuint array);
typedef void (*glDeleteVertexArrays_function) (GLsizei n, const GLuint *arrays);
//typedef void (*glDrawArrays_function) (GLenum mode, GLint first,	GLsizei count);
typedef void (*glDebugMessageCallback_function)(DEBUGPROC callback, void * userParam);

#define GlLoad(name) name = (name ##_function) wglGetProcAddress(#name); assert(name)

glGenBuffers_function glGenBuffers;
glBindBuffer_function glBindBuffer;
glDeleteBuffers_function glDeleteBuffers;
glBufferData_function glBufferData;
glVertexAttribPointer_function glVertexAttribPointer;
glEnableVertexAttribArray_function glEnableVertexAttribArray;
glDisableVertexAttribArray_function glDisableVertexAttribArray;
//glTexCoordPointer_function glTexCoordPointer;
//glColorPointer_function glColorPointer;
glGenVertexArrays_function glGenVertexArrays;
glBindVertexArray_function glBindVertexArray;
glDeleteVertexArrays_function glDeleteVertexArrays;
//glDrawArrays_function glDrawArrays;
glDebugMessageCallback_function glDebugMessageCallback;

GL_DEBUG_CALLBACK(GlHandleDebugMessage) {
	assert(0);
}

render_context MakeRenderContext() {
	GlLoad(glGenBuffers);
	GlLoad(glBindBuffer);
	GlLoad(glDeleteBuffers);
	GlLoad(glBufferData);
	GlLoad(glVertexAttribPointer);
	GlLoad(glEnableVertexAttribArray);
	GlLoad(glDisableVertexAttribArray);
//	GlLoad(glTexCoordPointer);
//	GlLoad(glColorPointer);
	GlLoad(glGenVertexArrays);
	GlLoad(glBindVertexArray);
	GlLoad(glDeleteVertexArrays);
//	GlLoad(glDrawArrays);
	GlLoad(glDebugMessageCallback);

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GlHandleDebugMessage, null);

	return {};
}

f32 Lerp(f32 a, f32 b, f32 blend_factor) {
	f32 result = a * (1 - blend_factor) + b * blend_factor;
	return result;
}

rgba32 Color32(f32 r, f32 g, f32 b, f32 a) {
	return {(u8) (r * 255), (u8) (g * 255), (u8) (b * 255), (u8) (a * 255)};
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

void DrawTexturedRect(render_context *renderer, texture my_texture, f32 x, f32 y, f32 tx, f32 ty, f32 width, f32 height, f32 x_alignment, f32 y_alignment, f32 x_flip, f32 y_flip, f32 alpha, f32 z) {
	auto command = renderer->commands + renderer->command_count; 
	renderer->command_count++;
	assert(renderer->command_count <= ArrayCountOf(renderer->commands));

	command->texture_id = my_texture.id;
	command->vertex_offset = renderer->vertex_count;
	command->vertex_count = 6;

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

	auto vertices = renderer->vertices + renderer->vertex_count;
	renderer->vertex_count += 6;
	assert(renderer->vertex_count <= ArrayCountOf(renderer->vertices));

	vertices[0].x = x * canvas_texel_width;
	vertices[0].y = y * canvas_texel_height;
	vertices[0].z = z;
	vertices[0].u = tx0;
	vertices[0].v = ty0;
	vertices[0].color = Color32(1.0f, 1.0f, 1.0f, alpha);

	vertices[1].x = (x + width) * canvas_texel_width;
	vertices[1].y = y * canvas_texel_height;
	vertices[1].z = z;
	vertices[1].u = tx1;
	vertices[1].v = ty0;
	vertices[1].color = Color32(1.0f, 1.0f, 1.0f, alpha);

	vertices[2].x = (x + width) * canvas_texel_width;
	vertices[2].y = (y + height) * canvas_texel_height;
	vertices[2].z = z;
	vertices[2].u = tx1;
	vertices[2].v = ty1;
	vertices[2].color = Color32(1.0f, 1.0f, 1.0f, alpha);

	vertices[3].x = x * canvas_texel_width;
	vertices[3].y = (y + height) * canvas_texel_height;
	vertices[3].z = z;
	vertices[3].u = tx0;
	vertices[3].v = ty1;
	vertices[3].color = Color32(1.0f, 1.0f, 1.0f, alpha);

	vertices[4] = vertices[0];
	vertices[5] = vertices[2];
#if 0
    glBegin(GL_TRIANGLES);
    glTexCoord2f(tx0, ty0);
    glColor4f(1, 1, 1, alpha);
    glVertex3f();
    
    glTexCoord2f(tx1, ty0);
    glVertex3f(, y * canvas_texel_height, z);
    
    glTexCoord2f(tx1, ty1);
    glVertex3f((x + width) * canvas_texel_width, (y + height) * canvas_texel_height, z);
    
    glTexCoord2f(tx0, ty1);
    glVertex3f(x * canvas_texel_width, (y + height) * canvas_texel_height, z);
    
    glTexCoord2f(tx0, ty0);
    glVertex3f(x * canvas_texel_width, y * canvas_texel_height, z);
    
    glTexCoord2f(tx1, ty1);
    glVertex3f((x + width) * canvas_texel_width, (y + height) * canvas_texel_height, z);
    glEnd();
#endif
}

void DrawTexture(render_context *renderer, texture my_texture, f32 x, f32 y, f32 x_alignment, f32 y_alignment, f32 x_flip, f32 y_flip, f32 alpha, f32 z) {
	DrawTexturedRect(renderer, my_texture, x, y, 0, 0, my_texture.width, my_texture.height, x_alignment, y_alignment, x_flip, y_flip, alpha, z);
}

void Render(render_context *renderer) {
	u32 vertex_buffer, vertex_array;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, renderer->vertex_count * sizeof(render_vertex), renderer->vertices, GL_STREAM_DRAW);
#if 0
	glEnableVertexAttribArray(vertex_attribute_index);
	glEnableVertexAttribArray(texture_attribute_index);
	glEnableVertexAttribArray(color_attribute_index);

	glVertexAttribPointer(vertex_attribute_index, 3, GL_FLOAT, GL_FALSE,sizeof(render_vertex), (void *) 0);
	glVertexAttribPointer(texture_attribute_index, 2, GL_FLOAT, GL_FALSE, sizeof(render_vertex), (void *) (3 * sizeof(f32)));
	glVertexAttribPointer(color_attribute_index, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(render_vertex), (void *) (5 * sizeof(f32)));
#else
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT,sizeof(render_vertex), (void *) 0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(render_vertex), (void *) (3 * sizeof(f32)));
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(render_vertex), (void *) (5 * sizeof(f32)));
#endif

	for (u32 i = 0; i < renderer->command_count; ++i) {
		glBindTexture(GL_TEXTURE_2D, renderer->commands[i].texture_id);
		glDrawArrays(GL_TRIANGLES, renderer->commands[i].vertex_offset, renderer->commands[i].vertex_count);
	}

	glDeleteBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	renderer->vertex_count = 0;
	renderer->command_count = 0;
}