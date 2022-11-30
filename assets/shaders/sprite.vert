struct Camera {
    vec3 world_pos;
    mat4 view_mat;
    mat4 proj_mat;
};

uniform Camera camera;
uniform vec4 tex_pos; // x, y, w, h
uniform mat4 world_mat;

out VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec3 world_norm;
    vec4 proj_pos;
    vec2 tex_pos;
} vs_out;


void main() {
    int id = gl_VertexID;

    mat4 view_mat = camera.view_mat;
    mat4 proj_mat = camera.proj_mat;

    float width = world_mat[0][0];
    float height = world_mat[1][1];

    vec4 w = world_mat[3];
    vec4 v = view_mat * w;
    vec2 t;

    // TODO: Refactor this if-statement to bit operation.
    if (id == 3) {
        v.x -= 0.5 * width;
        v.y -= 0.5 * height;
        t.x = tex_pos.x;
        t.y = tex_pos.y;
    } else if (id == 1) {
        v.x -= 0.5 * width;
        v.y += 0.5 * height;
        t.x = tex_pos.x;
        t.y = tex_pos.y + tex_pos.w;
    } else if (id == 2) {
        v.x += 0.5 * width;
        v.y -= 0.5 * height;
        t.x = tex_pos.x + tex_pos.z;
        t.y = tex_pos.y;
    } else if (id == 0) {
        v.x += 0.5 * width;
        v.y += 0.5 * height;
        t.x = tex_pos.x + tex_pos.z;
        t.y = tex_pos.y + tex_pos.w;
    }

    vec4 p = proj_mat * v;

    vs_out.proj_pos = p;
    vs_out.tex_pos = t;
    gl_Position = p;
}
