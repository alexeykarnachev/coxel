#version 460 core

in vec2 tex_pos;

uniform sampler2D image;

out vec4 f_color;

void main() {
    vec3 color = texture(image, tex_pos).rgb;
    color.r = max(color.r, 0.3);
    f_color = color;
}
