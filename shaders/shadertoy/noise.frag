#version 460 core

in vec2 tex_pos;

uniform vec2 u_resolution;
uniform float u_time;

out vec4 color;

float VALS256[256];
int INDS256[256];

float quintic_step(float x) {
    return x * x * x * (x * (x * 6. - 15.) + 10.);
}

float noise1d(float x) {
    int c1 = int(x) & 255;
    int c2 = (c1 + 1) & 255;

    float y1 = VALS256[c1];
    float y2 = VALS256[c2];

    float t = quintic_step(fract(x));
    float y = mix(y1, y2, t);
     
    return y;
}

float noise2d(float x, float y) {
    int cx1 = int(x) & 255;
    int cx2 = (cx1 + 1) & 255;
    int cy1 = int(y) & 255;
    int cy2 = (cy1 + 1) & 255;

    float z1 = VALS256[INDS256[(INDS256[cx1] + cy1) & 255]];
    float z2 = VALS256[INDS256[(INDS256[cx2] + cy1) & 255]];
    float z3 = VALS256[INDS256[(INDS256[cx1] + cy2) & 255]];
    float z4 = VALS256[INDS256[(INDS256[cx2] + cy2) & 255]];

    float tx = quintic_step(fract(x));
    float ty = quintic_step(fract(y));

    float zx1 = mix(z1, z2, tx);
    float zx2 = mix(z3, z4, tx);
    float z = mix(zx1, zx2, ty);

    return z;
}

float noise3d(float x, float y, float z) {
    int cx1 = int(x) & 255;
    int cx2 = (cx1 + 1) & 255;
    int cy1 = int(y) & 255;
    int cy2 = (cy1 + 1) & 255;
    int cz1 = int(z) & 255;
    int cz2 = (cz1 + 1) & 255;

    float w1 = VALS256[
        (INDS256[(INDS256[cx1] + cy1) & 255] + cz1) & 255];
    float w2 = VALS256[
        (INDS256[(INDS256[cx1] + cy1) & 255] + cz2) & 255];
    float w3 = VALS256[
        (INDS256[(INDS256[cx2] + cy1) & 255] + cz1) & 255];
    float w4 = VALS256[
        (INDS256[(INDS256[cx2] + cy1) & 255] + cz2) & 255];
    float w5 = VALS256[
        (INDS256[(INDS256[cx1] + cy2) & 255] + cz1) & 255];
    float w6 = VALS256[
        (INDS256[(INDS256[cx1] + cy2) & 255] + cz2) & 255];
    float w7 = VALS256[
        (INDS256[(INDS256[cx2] + cy2) & 255] + cz1) & 255];
    float w8 = VALS256[
        (INDS256[(INDS256[cx2] + cy2) & 255] + cz2) & 255];

    float tx = quintic_step(fract(x));
    float ty = quintic_step(fract(y));
    float tz = quintic_step(fract(z));

    float wx1 = mix(w1, w3, tx);
    float wx2 = mix(w2, w4, tx);
    float wx3 = mix(w5, w7, tx);
    float wx4 = mix(w6, w8, tx);

    float wy1 = mix(wx1, wx3, ty);
    float wy2 = mix(wx2, wx4, ty);

    float w = mix(wy1, wy2, tz);

    return w;
}

void main(void) {
    float x = tex_pos.x * 20.0;
    float y = tex_pos.y * 20.0;
    float z = u_time * 0.5;
    float w = noise3d(x, y, z);
    color = vec4(vec3(w), 1.0);
}
