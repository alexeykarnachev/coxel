#version 460 core

in vec2 tex_pos;

layout(binding = 0) uniform sampler2D orig_image;
layout(binding = 1) uniform sampler2D blur_image;
uniform float gamma = 2.2;
uniform float exposure = 5.0;

out vec4 f_color;

vec3 hdr(vec3 color, float gamma, float exposure);

void main() {             
    vec3 orig_color = texture(orig_image, tex_pos).rgb;
    vec3 blur_color = texture(blur_image, tex_pos).rgb;
    vec3 color = orig_color + blur_color;
    f_color = vec4(hdr(color, gamma, exposure), 1.0);
}

