shader::vertex {
#version 410

in vec3 position;

uniform mat4 pvm_matrix;
uniform vec3 camera_pos;

void main() {
  
  gl_Position = pvm_matrix * vec4(position, 1);
}
} shader::vertex

shader::fragment {
#version 410

out vec4 output_color;
uniform vec3 color;

void main() {
    output_color = vec4(color, 1);
}
} shader::fragment