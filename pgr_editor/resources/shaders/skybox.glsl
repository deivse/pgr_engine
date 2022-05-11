shader::vertex {
#version 410

in vec3 position;

uniform mat4 skybox_matrix;

smooth out vec3 v_tex_coords;

void main() {
  gl_Position = skybox_matrix * vec4(position, 1);
  gl_Position.w = gl_Position.z + 0.0000001;
  v_tex_coords = position;
}
} shader::vertex

shader::fragment {
#version 410

out vec4 output_color;
smooth in vec3 v_tex_coords;

uniform samplerCube cubemap;

void main() {
    output_color = texture(cubemap, v_tex_coords);
}
} shader::fragment