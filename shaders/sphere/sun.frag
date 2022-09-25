#version 460 core

in vec4 es_m_pos;
in vec4 es_m_center_pos;

uniform vec3 u_color;

layout (location = 0) out vec4 f_color;
layout (location = 1) out vec4 f_brightness; 

void main() {
    f_color = vec4(u_color, 1.0);
    f_brightness = f_color;
}

