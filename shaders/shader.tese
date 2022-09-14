#version 460 core

layout(triangles, equal_spacing, ccw) in;
in vec3 cs_out_pos[];

uniform mat4 u_view;
uniform mat4 u_proj;

void main(void) {
    float x = mix(cs_out_pos[0].x, cs_out_pos[2].x, gl_TessCoord.x);
    float y = mix(cs_out_pos[0].y, cs_out_pos[1].y, gl_TessCoord.y);
    float z = mix(cs_out_pos[0].z, cs_out_pos[2].z, gl_TessCoord.z);
    float w = mix(cs_out_pos[0].w, cs_out_pos[2].w, gl_TessCoord.z);

    gl_Position = u_proj * u_view * vec4(x, y, z, w);
}
