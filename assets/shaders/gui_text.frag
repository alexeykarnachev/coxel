in vec2 tex_pos;

uniform sampler2D font_tex;
uniform vec3 color;

layout(location=2) out vec4 gui_text_tex;

void main() {
    ivec2 tex_size = textureSize(font_tex, 0);

    float u = tex_pos.x;
    float v = tex_pos.y;
    float dudx = dFdx(u);
    float dvdx = dFdx(v);

    float u_min = max(0, u - 0.5 * dudx);
    float u_max = min(1, u + 0.5 * dudx);
    if (floor(u_min) == floor(u_max)) {
        u = (u_min + u_max) / 2;
    }

    float v_min = max(0, v - 0.5 * dvdx);
    float v_max = min(1, v + 0.5 * dvdx);
    if (floor(v_min) == floor(v_max)) {
        v = (v_min + v_max) / 2;
    }

    float alpha = texture(font_tex, vec2(u, v)).r;
    gui_text_tex = vec4(color, alpha);
}



