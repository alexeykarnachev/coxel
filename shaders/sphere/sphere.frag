#version 460 core

in vec4 es_pos;

uniform vec3 u_center_pos;
uniform vec3 u_light_pos;

out vec4 f_color;

const vec3 diffuse_color = vec3(0.6, 0.6, 1.0);

void main() {
    vec3 light_dir = normalize(es_pos.xyz - u_light_pos);
    vec3 normal = normalize(es_pos.xyz - u_center_pos);

    float lambertian = max(dot(-light_dir, normal), 0.0);
    vec3 color = lambertian * diffuse_color;
    f_color = vec4(color, 1.0);
}
