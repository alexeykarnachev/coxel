in vec3 model_pos;

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
    vec4 proj_pos;
} vertex_data;


void main() {
    vec4 m = vec4(model_pos, 1.0);
    vec4 w = world_mat * m; 
    vec4 p = camera.proj_mat * camera.view_mat * w; 

    vertex_data.model_pos = m;
    vertex_data.world_pos = w;
    vertex_data.proj_pos = p;
    gl_Position = p;
}

