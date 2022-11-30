in vec2 tex_pos;

struct Camera {
    vec4 world_pos;
    mat4 view_mat;
    mat4 proj_mat;
};

struct PointLight {
    vec4 world_pos;
    vec4 color;
    float energy;
};

uniform Camera camera;
uniform PointLight point_lights[MAX_N_POINT_LIGHTS_TO_RENDER];
uniform int n_point_lights;
uniform sampler2D world_pos_tex;
uniform sampler2D world_norm_tex;
uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;

out vec4 frag_color;


void main() {
    // vec3 camera_world_pos = camera.world_pos.xyz;

    // vec3 world_pos = texture(world_pos_tex, tex_pos);
    // vec3 view_dir = normalize(world_pos - camera_world_pos);
    vec3 diffuse_color = texture(diffuse_tex, tex_pos).rgb;

    frag_color = vec4(diffuse_color, 1.0);
}
