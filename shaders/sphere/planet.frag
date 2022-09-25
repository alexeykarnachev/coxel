#version 460 core

in vec4 es_pos;

uniform vec3 u_center_pos;
uniform vec3 u_light_pos;
uniform vec3 u_diffuse_color;
uniform vec3 u_ambient_color;
uniform vec3 u_light_color;

layout(location = 0) out vec4 f_color;
layout(location = 1) out vec4 f_brightness;

float perlin_noise3d(float x, float y, float z);


void main() {
    vec3 light_dir = es_pos.xyz - u_light_pos;
    float light_dist = length(light_dir);
    light_dir = normalize(light_dir);
    vec3 normal = normalize(es_pos.xyz - u_center_pos);

    float lambertian = max(dot(-light_dir, normal), 0.0);
    float n = perlin_noise3d(es_pos.x, es_pos.y, es_pos.z);
    vec3 diffuse_term = n * lambertian * u_diffuse_color * u_light_color / (light_dist * light_dist);
    vec3 color = u_ambient_color + diffuse_term;
    f_color = vec4(color, 1.0);

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    f_brightness = brightness > 1.0 ? f_color : vec4(0.0, 0.0, 0.0, 1.0);
}
