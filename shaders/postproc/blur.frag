#version 460 core

in vec2 tex_pos;

layout(binding = 0) uniform sampler2D image;
uniform bool u_is_horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

out vec4 f_color;

void main() {
    vec2 tex_offset = 1.0 / textureSize(image, 0);
    vec3 result = texture(image, tex_pos).rgb * weight[0];

    if (u_is_horizontal) {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, tex_pos + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, tex_pos - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    } else {
        for (int i = 1; i < 5; ++i) {
            result += texture(image, tex_pos + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, tex_pos - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }

    f_color = vec4(result, 1.0);
}

