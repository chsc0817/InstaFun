#version 330
in vec2 fragment_texture_coordinate;
in vec4 fragment_color;

out vec4 pixel_color;

void main() {
	pixel_color = vec4(1, 0, 0, 1);
}