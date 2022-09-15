# version 460 core

layout(triangles, equal_spacing, ccw) in;
in vec3 cs_pos[];

uniform mat4 u_mv;
uniform mat4 u_proj;

void main(void) {
    vec3 p0 = gl_TessCoord.x * cs_pos[0];
    vec3 p1 = gl_TessCoord.y * cs_pos[1];
    vec3 p2 = gl_TessCoord.z * cs_pos[2];

    vec3 pos = normalize(p0 + p1 + p2);

    gl_Position = u_proj * u_mv * vec4(pos, 1.0);
}
