shader::vertex {
#version 410

in vec3 position;

uniform mat4 pvm_matrix;
uniform vec3 camera_pos;

smooth out float dist_to_cam;

void main() {
  
  gl_Position = pvm_matrix * vec4(position, 1);
  dist_to_cam = distance(position, camera_pos);
}
} shader::vertex

shader::fragment {
#version 410

out vec4 output_color;
smooth in float dist_to_cam;
uniform vec3 color;

void main() {
    output_color = vec4(color, 1);
}
} shader::fragment