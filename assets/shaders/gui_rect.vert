uniform mat4 world_mat;
uniform float width;
uniform float height;
uniform int layer;
uniform int buffer_width;
uniform int buffer_height;

out VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec3 world_norm;
    vec4 proj_pos;
    vec2 tex_pos;
} vs_out;


void main() {
    int id = gl_VertexID;

    float x = world_mat[3][0] / float(buffer_width);
    float y = (float(buffer_height) - world_mat[3][1]) / float(buffer_height);
    float z = float(layer) / MAX_N_GUI_LAYERS;
    float w = width / float(buffer_width);
    float h = height / float(buffer_height);

    float x0 = (x * 2.0) - 1.0;
    float y0 = (y * 2.0) - 1.0;
    float x1 = x0 + w * 2.0;
    float y1 = y0 - h * 2.0;

    if (id == 0) {
        vs_out.proj_pos = vec4(x0, y0, z, 1);
    } else if (id == 1) {
        vs_out.proj_pos = vec4(x1, y0, z, 1);
    } else if (id == 2) {
        vs_out.proj_pos = vec4(x0, y1, z, 1);
    } else if (id == 3) {
        vs_out.proj_pos = vec4(x1, y1, z, 1);
    }

    gl_Position = vs_out.proj_pos;
}
