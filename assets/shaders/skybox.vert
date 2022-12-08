struct Camera {
    vec4 world_pos;
    mat4 view_mat;
    mat4 proj_mat;
};

uniform Camera camera;

const vec3 skybox_strip[14] = {
    vec3(-1.f, 1.f, 1.f),
    vec3(1.f, 1.f, 1.f),
    vec3(-1.f, -1.f, 1.f),
    vec3(1.f, -1.f, 1.f),
    vec3(1.f, -1.f, -1.f),
    vec3(1.f, 1.f, 1.f),
    vec3(1.f, 1.f, -1.f),
    vec3(-1.f, 1.f, 1.f),
    vec3(-1.f, 1.f, -1.f),
    vec3(-1.f, -1.f, 1.f),
    vec3(-1.f, -1.f, -1.f),
    vec3(1.f, -1.f, -1.f),
    vec3(-1.f, 1.f, -1.f),
    vec3(1.f, 1.f, -1.f)
};


void main() {
    mat4 vp = camera.proj_mat * mat4(mat3(camera.view_mat));
    vec4 pos = vp * vec4(skybox_strip[gl_VertexID], 1.0);
    gl_Position = pos.xyww;
}
