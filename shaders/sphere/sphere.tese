# version 460 core

layout(triangles, equal_spacing, ccw) in;
in vec3 cs_pos[];

uniform mat4 u_mv;
uniform mat4 u_proj;
uniform vec4 u_v_center;
uniform vec4 u_v_light_pos;

out vec3 es_normal;
out vec3 es_light_dir;

void main(void) {
    vec3 p0 = gl_TessCoord.x * cs_pos[0];
    vec3 p1 = gl_TessCoord.y * cs_pos[1];
    vec3 p2 = gl_TessCoord.z * cs_pos[2];

    vec4 pos = vec4(normalize(p0 + p1 + p2), 1.0);
    vec4 mv_pos = u_mv * pos;

    es_light_dir = normalize((mv_pos - u_v_light_pos).xyz);
    es_normal = normalize((mv_pos - u_v_center).xyz);
    gl_Position = u_proj * u_mv * pos;
}
