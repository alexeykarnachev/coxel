#version 460 core

in vec3 es_out_pos;

uniform vec3 u_center_pos;
uniform vec3 u_light_pos;

out vec4 f_color;

void main() {
    // vec3 light_dir = normalize(u_center_pos - u_light_pos);
    // vec3 normal = normalize(es_out_pos - u_center_pos);

    vec3 light_dir = vec3(0.0, 0.0, -1.0);
    vec3 normal = vec3(0.0, 0.0, 1.0);
    float c = dot(normal, -light_dir);
    f_color = vec4(vec3(c), 1.0);
}
