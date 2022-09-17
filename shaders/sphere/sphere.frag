#version 460 core

out vec4 f_color;

in vec3 es_normal;
in vec3 es_light_dir;

void main() {
    float light = dot(es_normal, -es_light_dir);
    f_color = vec4(vec3(light), 1.0);
}
