in vec3 model_pos;

uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

out VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} vs_out;


void main() {
    vec4 m = vec4(model_pos, 1.0);
    vec4 w = world_mat * m; 
    vec4 p = proj_mat * view_mat * w; 

    vs_out.model_pos = m;
    vs_out.world_pos = w;
    vs_out.proj_pos = p;
    gl_Position = p;
}

