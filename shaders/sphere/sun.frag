#version 460 core

in vec4 es_m_pos;
in vec4 es_m_center_pos;

uniform vec3 u_color;

layout (location = 0) out vec4 f_color;
layout (location = 1) out vec4 f_brightness; 

void main() {
    vec3 color = u_color;

    f_color = vec4(color, 1.0);
    f_brightness = vec4(0.0);
}

