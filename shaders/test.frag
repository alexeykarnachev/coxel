#version 460 core

in vec2 tex_pos;

uniform sampler2D tex;

out vec4 f_color;

void main() {
    vec4 color = texture(tex, tex_pos);
    color.r = max(color.r, 0.3);
    f_color = color;
}
