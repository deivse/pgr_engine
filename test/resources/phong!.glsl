shader::vertex {
#version 410

// IMPORTANT: !!! lighting is evaluated in camera space !!!

struct Material {      // structure that describes currently used material
  vec3  ambient;       // ambient component
  vec3  diffuse;       // diffuse component
  vec3  specular;      // specular component
  float shininess;     // sharpness of specular reflection
  float transparency;

  bool  use_texture;    // defines whether the texture is used or not
};

uniform sampler2D colorTexSampler;  // sampler for the texture access

struct PointLight {
    vec3  ambient;       // intensity & color of the ambient component
    vec3  diffuse;       // intensity & color of the diffuse component
    vec3  specular;      // intensity & color of the specular component
    vec3  position;      // light position
    vec3  attenuation;
};

struct SpotLight {         // structure describing light parameters
  vec3  ambient;       // intensity & color of the ambient component
  vec3  diffuse;       // intensity & color of the diffuse component
  vec3  specular;      // intensity & color of the specular component
  vec3  position;      // light position
  vec3  direction;     // spotlight direction
  float cos_half_angle; // cosine of the spotlight's half angle
  float exponent;  // distribution of the light energy within the reflector's cone (center->cone's edge)
};

struct SunLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
};

in vec3 position;           // vertex position in world space
in vec3 normal;             // vertex normal
in vec2 tex_coord;           // incoming texture coordinates

uniform float time;         // time used for simulation of moving lights (such as sun)
uniform Material material;  // current material

#define MAX_SUN_LIGHTS 2
uniform SunLight sun_lights[MAX_SUN_LIGHTS];
#define MAX_POINT_LIGHTS 50
uniform PointLight point_lights[MAX_POINT_LIGHTS];
#define MAX_SPOT_LIGHTS 50
uniform SpotLight spot_lights[MAX_SPOT_LIGHTS];

uniform int num_sun_lights;
uniform int num_point_lights;
uniform int num_spot_lights;

uniform mat4 pvm_matrix;     // Projection * View * Model  --> model to clip coordinates
uniform mat4 view_matrix;       // View                       --> world to eye coordinates
uniform mat4 model_matrix;       // Model                      --> model to world coordinates
uniform mat4 normal_matrix;  // inverse transposed model_matrix

smooth out vec2 tex_coord_v;  // outgoing texture coordinates
smooth out vec4 color_v;     // outgoing fragment color

vec4 calc_spot(SpotLight light, vec3 ver_pos, vec3 ver_normal) {

  vec3 ret = vec3(0.0);

  // use the material and light structures to obtain the surface and light properties
  // the ver_pos and ver_normal variables contain transformed surface position and normal
  // store the ambient, diffuse and specular terms to the ret variable
  // for spot lights, light.position contains the light position
  // everything is expressed in the view coordinate system -> eye/camera is in the origin

  vec3 L = normalize(light.position - ver_pos);
  vec3 R = reflect(-L, ver_normal);
  vec3 V = normalize(-ver_pos);
  float NdotL = max(0.0, dot(ver_normal, L));
  float RdotV = max(0.0, dot(R, V));
  float spotCoef = max(0.0, dot(-L, light.direction));

  ret += material.ambient * light.ambient;
  ret += material.diffuse * light.diffuse * NdotL;
  ret += material.specular * light.specular * pow(RdotV, material.shininess);

  if(spotCoef < light.cos_half_angle)
    ret *= 0.0;
  else
    ret *= pow(spotCoef, light.exponent);

  return vec4(ret, 1.0);
}

vec4 calc_sun(SunLight light, vec3 ver_pos, vec3 ver_normal) {

  vec3 ret = vec3(0.0);

  // use the material and light structures to obtain the surface and light properties
  // the ver_pos and ver_normal variables contain transformed surface position and normal
  // store the ambient, diffuse and specular terms to the ret variable
  // glsl provides some built-in functions, for example: reflect, normalize, pow, dot
  // for directional lights, light.position contains the direction
  // everything is expressed in the view coordinate system -> eye/camera is in the origin

  vec3 L = normalize(light.direction);
  vec3 R = reflect(-L, ver_normal);
  vec3 V = normalize(-ver_pos);
  float NdotL = max(0.0, dot(ver_normal, L));
  float RdotV = max(0.0, dot(R, V));

  ret += material.ambient * light.ambient;
  ret += material.diffuse * light.diffuse * NdotL;
  ret += material.specular * light.specular * pow(RdotV, material.shininess);

  return vec4(ret, 1.0);
}

void main() {
  // eye-coordinates position and normal of vertex
  vec3 ver_pos = (view_matrix * model_matrix * vec4(position, 1.0)).xyz;                // vertex in eye coordinates
  vec3 ver_normal = normalize( (normal_matrix * vec4(normal, 0.0)).xyz);   // normal in eye coordinates by normal_matrix

  vec3 gloabl_ambient_l = vec3(0.8f);
  vec4 output_color = vec4(material.ambient * gloabl_ambient_l, 0.0);

  for(int i=0;i<num_sun_lights;++i)
  {
    output_color += calc_sun(sun_lights[i], ver_pos, ver_normal);
  }
  for(int i=0;i< num_spot_lights;++i)
  {
    output_color += calc_spot(spot_lights[i], ver_pos, ver_normal);
  }

  //TODO: point lights
  color_v = output_color;
  tex_coord_v = tex_coord;

  gl_Position = pvm_matrix * vec4(position, 1);

  // outputs entering the fragment shader
}
} shader::vertex

shader::fragment {
#version 410

struct Material {      // structure that describes currently used material
  vec3  ambient;       // ambient component
  vec3  diffuse;       // diffuse component
  vec3  specular;      // specular component
  float shininess;     // sharpness of specular reflection
  float transparency;

  bool  use_texture;    // defines whether the texture is used or not
};

uniform sampler2D colorTexSampler;  // sampler for the texture access

uniform Material material;     // current material

smooth in vec4 color_v;        // incoming fragment color (includes lighting)
smooth in vec2 tex_coord_v;    // fragment texture coordinates
out vec4       color_f;        // outgoing fragment color

void main() {
  color_f = color_v;

  // if material has a texture -> apply it
  if(material.use_texture)
    color_f =  color_v * texture(colorTexSampler, tex_coord_v);
}
} shader::fragment
