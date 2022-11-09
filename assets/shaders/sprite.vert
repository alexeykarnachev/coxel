struct Camera {
    vec3 world_pos;
    mat4 view_mat;
    mat4 proj_mat;
};

uniform Camera camera;
uniform vec4 tex_pos; // x, y, w, h
uniform vec3 world_pos; // x, y, z
uniform vec3 world_size; // w, h

out VertexData {
    vec4 proj_pos;
    vec2 tex_pos;
} vertex_data;


void main() {
    int id = gl_VertexID;

    mat4 view_mat = camera.view_mat;
    mat4 proj_mat = camera.proj_mat;

    vec4 w = vec4(world_pos, 1);
    vec4 v = view_mat * w;
    vec2 t;

    // TODO: Refactor this if-statement to bit operation.
    if (id == 3) {
        v.x -= 0.5 * world_size.x;
        v.y -= 0.5 * world_size.y;
        t.x = tex_pos.x;
        t.y = tex_pos.y;
    } else if (id == 1) {
        v.x -= 0.5 * world_size.x;
        v.y += 0.5 * world_size.y;
        t.x = tex_pos.x;
        t.y = tex_pos.y + tex_pos.w;
    } else if (id == 2) {
        v.x += 0.5 * world_size.x;
        v.y -= 0.5 * world_size.y;
        t.x = tex_pos.x + tex_pos.z;
        t.y = tex_pos.y;
    } else if (id == 0) {
        v.x += 0.5 * world_size.x;
        v.y += 0.5 * world_size.y;
        t.x = tex_pos.x + tex_pos.z;
        t.y = tex_pos.y + tex_pos.w;
    }

    vec4 p = proj_mat * v;

    vertex_data.proj_pos = p;
    vertex_data.tex_pos = t;
    gl_Position = p;
}
