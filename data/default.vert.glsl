#version 330
in vec3 vertex_position;
in vec2 vertex_texture_coordinate;
in vec4 vertex_color;

out vec2 fragment_texture_coordinate;
out vec4 fragment_color;

void main() {
	fragment_texture_coordinate = vertex_texture_coordinate;
	fragment_color = vertex_color;
	gl_Position = vec4(vertex_position, 1);
}