#version 460 core

in vec2 tex_pos;

layout(binding = 0) uniform sampler2D tex0;
layout(binding = 1) uniform sampler2D tex1;

out vec4 f_color;

void main() {             
    vec3 color0 = texture(tex0, tex_pos).rgb;
    vec3 color1 = texture(tex1, tex_pos).rgb;
    vec3 color = color0 + color1;

    // const float gamma = 2.2;
    // const float exposure = 4.0;

    // color = vec3(1.0) - exp(-color * exposure);
    // color = pow(color, vec3(1.0 / gamma));

    f_color = vec4(color, 1.0);
}
