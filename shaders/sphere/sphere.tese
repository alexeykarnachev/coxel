#version 460 core

layout(triangles, equal_spacing, ccw) in;
in vec3 cs_out_pos[];

uniform mat4 u_mv;
uniform mat4 u_proj;

out vec3 es_out_pos;

void main(void) {
    vec3 p0 = gl_TessCoord.x * cs_out_pos[0];
    vec3 p1 = gl_TessCoord.y * cs_out_pos[1];
    vec3 p2 = gl_TessCoord.z * cs_out_pos[2];
    es_out_pos = normalize(p0 + p1 + p2);

    gl_Position = u_proj * u_mv * vec4(es_out_pos, 1.0);
}
