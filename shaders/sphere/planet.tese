# version 460 core

layout(triangles, equal_spacing, ccw) in;
in vec3 cs_pos[];

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

out vec4 es_pos;

float perlin_noise3d(float x, float y, float z);

void main(void) {
    vec3 p0 = gl_TessCoord.x * cs_pos[0];
    vec3 p1 = gl_TessCoord.y * cs_pos[1];
    vec3 p2 = gl_TessCoord.z * cs_pos[2];

    vec3 p = normalize(p0 + p1 + p2);
    float noise = perlin_noise3d(p.x * 10, p.y * 10, p.z * 10);
    p += (noise * 0.05);

    vec4 pos = vec4(p, 1.0);
    es_pos = u_model * pos; 
    gl_Position = u_proj * u_view * es_pos;
}
