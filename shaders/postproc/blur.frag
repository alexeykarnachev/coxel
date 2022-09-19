#version 460 core

in vec2 tex_pos;

layout(binding = 0) uniform sampler2D image;
uniform bool u_is_horizontal;

out vec4 f_color;

uniform float[10] weights = {0.13298076013386395, 0.1257944092310479, 0.10648266850749323, 0.08065690817307997, 0.05467002489201969, 0.0331590462642628, 0.017996988837736535, 0.008740629697906652, 0.0037986620079339963, 0.001477282803979925};

void main() {
    vec2 step_size = 1.0 / textureSize(image, 0);
    f_color = texture(image, tex_pos) * weights[0];

    if (u_is_horizontal) {
        for(int i = 1; i < 10; ++i) {
            f_color += texture(image, tex_pos + vec2(i * step_size.x, 0)) * weights[i];
            f_color += texture(image, tex_pos - vec2(i * step_size.x, 0)) * weights[i];
        }
    } else {
        for(int i = 1; i < 10; ++i) {
            f_color += texture(image, tex_pos + vec2(0, i * step_size.y)) * weights[i];
            f_color += texture(image, tex_pos - vec2(0, i * step_size.y)) * weights[i];
        }
    }
}

