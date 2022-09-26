#version 460 core

in vec2 vs_tex_pos;

layout(binding = 0) uniform sampler2D u_tex;
uniform float u_gamma = 2.2;
uniform float u_exposure = 5.0;

out vec4 f_color;


vec3 hdr(vec3 color, float gamma, float exposure);


void main(void) {
    vec4 color = texture(u_tex, vs_tex_pos);
    vec3 hdr_color = hdr(color.rgb, u_gamma, u_exposure);
    f_color = vec4(hdr_color, 1.0);
}
