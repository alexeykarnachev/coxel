in vec3 model_pos;
in vec3 model_norm;

struct Camera {
    vec4 world_pos;
    mat4 view_mat;
    mat4 proj_mat;
};

uniform mat4 world_mat;
uniform Camera camera;

out VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec3 world_norm;
    vec4 proj_pos;
    vec2 tex_pos;
} vs_out;


void main() {
    vec4 m = vec4(model_pos, 1.0);
    vec4 w = world_mat * m; 
    vec4 p = camera.proj_mat * camera.view_mat * w; 

    mat3 norm_mat = transpose(inverse(mat3(world_mat)));
    vec3 n = normalize(norm_mat * model_norm);

    vs_out.model_pos = m;
    vs_out.world_pos = w;
    vs_out.proj_pos = p;
    vs_out.world_norm = n;
    gl_Position = p;
}

