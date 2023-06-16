shader::fragment {
#version 410

struct Material {      // structure that describes currently used material
  vec3  ambient;       // ambient component
  vec3  diffuse;       // diffuse component
  vec3  specular;      // specular component
  float shininess;     // sharpness of specular reflection
  float opacity;

  bool use_texture;    // defines whether the texture is used or not
  float tex_coord_anim_speed;  
  bool spritesheet;
  float spritesheet_frame_duration;
  ivec2 spritesheet_dims;
};

struct FogSettings {
  vec4 color;
  float density;
  bool enable;
};

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
  float exponent;       // distribution of the light energy within the reflector's cone (center->cone's edge)
  vec3  attenuation;
};

struct SunLight {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  vec3 direction;
};

#define MAX_SUN_LIGHTS 2
uniform SunLight sun_lights[MAX_SUN_LIGHTS];
#define MAX_POINT_LIGHTS 50
uniform PointLight point_lights[MAX_POINT_LIGHTS];
#define MAX_SPOT_LIGHTS 50
uniform SpotLight spot_lights[MAX_SPOT_LIGHTS];

uniform int num_sun_lights;
uniform int num_point_lights;
uniform int num_spot_lights;

uniform mat4 pvm_matrix; 
uniform mat4 view_matrix;
// uniform mat4 model_matrix;
uniform mat4 normal_matrix;  // inverse transposed model_matrix

uniform sampler2D color_tex_sampler;
uniform double time;

uniform Material material;
uniform FogSettings fog;
    
smooth in vec2 v_tex_coord;
smooth in vec3 v_position_cam;
smooth in vec3 v_normal_cam;

out vec4       output_color;

float calc_dist_attenuation(vec3 light_att, float dist_to_light) {
    return 1.0 / (light_att[0] 
        + light_att[1] * dist_to_light 
        + light_att[2] * pow(dist_to_light, 2));
}

vec4 add_fog(vec4 color) {
    float fog_factor = (fog.density* distance(vec3(0), v_position_cam))/100000;
    return mix(clamp(color, 0.0, 1.0), fog.color, clamp(fog_factor, 0.0, 1.0));
    // return vec4(vec3(distance(vec3(0), v_position_cam)/1000), 1);
}

vec3 calc_point_l(PointLight light, vec3 ver_pos_cam, vec3 ver_normal_cam){
    vec3 light_pos_cam = (view_matrix * vec4(light.position, 1.0)).xyz;
    float dist_to_light = distance(light_pos_cam, ver_pos_cam);
    float dist_attenuation = calc_dist_attenuation(light.attenuation, dist_to_light);

    vec3 light_dir = normalize(light_pos_cam - ver_pos_cam);
    vec3 reflected_light_dir = reflect(-light_dir, ver_normal_cam);

    float cos_light_normal = max(0, dot(ver_normal_cam, light_dir));
    float cos_reflected_view = max(0, dot(reflected_light_dir, normalize(-ver_pos_cam)));

    return dist_attenuation * ( material.ambient * light.ambient 
                              + material.diffuse * light.diffuse * cos_light_normal
                              + float(bool(cos_light_normal)) * material.specular * light.specular
                                * pow(cos_reflected_view, material.shininess) );
}

vec3 calc_sun_l(SunLight light, vec3 ver_pos_cam, vec3 ver_normal_cam){
    vec3 light_dir = normalize(view_matrix * vec4(light.direction, 0.0)).xyz;
    vec3 reflected_light_dir = reflect(-light_dir, ver_normal_cam);
    
    float cos_light_normal = max(0.0, dot(ver_normal_cam, light_dir));
    float cos_reflected_view = max(0.0, dot(reflected_light_dir, normalize(-ver_pos_cam)));

    return ( material.ambient * light.ambient 
           + material.diffuse * light.diffuse * cos_light_normal
           + material.specular * light.specular
             * pow(cos_reflected_view, material.shininess) );
}

vec3 calc_spot_l(SpotLight light, vec3 ver_pos_cam, vec3 ver_normal_cam){
    vec3 light_pos_cam = (view_matrix * vec4(light.position, 1.0)).xyz;
    vec3 spot_light_dir_cam = normalize(view_matrix * vec4(light.direction, 0.0)).xyz;

    float dist_to_light = distance(light_pos_cam, ver_pos_cam);
    float dist_attenuation = calc_dist_attenuation(light.attenuation, dist_to_light);

    vec3 light_dir = normalize(light_pos_cam - ver_pos_cam);
    vec3 reflected_light_dir = reflect(-light_dir, ver_normal_cam);

    float cos_light_normal = max(0.0, dot(ver_normal_cam, light_dir));
    float cos_reflected_view = max(0.0, dot(reflected_light_dir, normalize(-ver_pos_cam)));
    float spot_coef = max(0.0, dot(-light_dir, spot_light_dir_cam));
    float spot_attenuation;

    if (spot_coef < light.cos_half_angle) {
        spot_attenuation = 0.0;
    } else {
        spot_attenuation = pow(spot_coef, light.exponent);
    }

    return spot_attenuation * dist_attenuation 
           * ( material.ambient * light.ambient 
             + material.diffuse * light.diffuse * cos_light_normal
             + float(bool(cos_light_normal)) * material.specular * light.specular
               * pow(cos_reflected_view, material.shininess) );
}

void main() {
  vec3 normal_cam = normalize(v_normal_cam);

  vec3 color = vec3(0);
  
  for(int i=0;i<num_point_lights;++i)
  {
    color += calc_point_l(point_lights[i], v_position_cam, normal_cam);
  }
  for(int i=0;i<num_sun_lights;++i)
  {
    color += calc_sun_l(sun_lights[i], v_position_cam, normal_cam);
  }
  for(int i=0;i< num_spot_lights;++i)
  {
    color += calc_spot_l(spot_lights[i], v_position_cam, normal_cam);
  }
  output_color = vec4(color, material.opacity);

  if(material.use_texture) {
    vec2 tex_coord = v_tex_coord;
    tex_coord.x += float(time * material.tex_coord_anim_speed);
    tex_coord.y += float(time * material.tex_coord_anim_speed);

    if (material.spritesheet) {
        vec2 offset = vec2(1.0) / vec2(material.spritesheet_dims);
        int frame = int(time / material.spritesheet_frame_duration);
        vec2 tmp = tex_coord / vec2(material.spritesheet_dims);
        tex_coord
          = tmp
            + vec2(frame % material.spritesheet_dims.x,
                   material.spritesheet_dims.y - 1 - (frame / material.spritesheet_dims.x))
                * offset;
    }
    output_color *= texture(color_tex_sampler, tex_coord);
  }
  if(fog.enable) {
    output_color = add_fog(output_color);
  }
}
} shader::fragment

shader::vertex {
#version 410

layout (location = 0) in vec3 position;           // vertex position in world space
layout (location = 1) in vec3 normal;             // vertex normal
layout (location = 2) in vec2 tex_coord;          // incoming texture coordinates

uniform mat4 pvm_matrix; 
uniform mat4 vm_matrix;
uniform mat4 v_normal_matrix; 

smooth out vec2 v_tex_coord;  // texture coordinates
smooth out vec3 v_position_cam;   // fragment coordinates
smooth out vec3 v_normal_cam;

void main() {
  gl_Position = pvm_matrix * vec4(position, 1);

  v_tex_coord = tex_coord;
  v_position_cam = (vm_matrix * vec4(position, 1)).xyz;
  v_normal_cam = (v_normal_matrix * vec4(normal, 0)).xyz;
}
} shader::vertex
