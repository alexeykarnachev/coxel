in vec3 model_pos;

uniform int camera_id;
uniform int transformation_id;

struct Transformation {
    mat4 world_mat;
};

struct Camera {
    vec3 world_pos;
    mat4 view_mat;
    mat4 proj_mat;
};

layout (std140, binding=TRANSFORMATION_BINDING_IDX) uniform Transformations {
    Transformation transformations[MAX_N_TRANSFORMATIONS];
    int n_transformations;
};

layout (std140, binding=CAMERA_BINDING_IDX) uniform Cameras {
    Camera cameras[MAX_N_CAMERAS];
    int n_cameras;
};

out VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} vs_out;


void main() {
    mat4 world_mat = transformations[transformation_id].world_mat;
    mat4 view_mat = cameras[camera_id].view_mat;
    mat4 proj_mat = cameras[camera_id].proj_mat;

    vec4 m = vec4(model_pos, 1.0);
    vec4 w = world_mat * m; 
    vec4 p = proj_mat * view_mat * w; 

    vs_out.model_pos = m;
    vs_out.world_pos = w;
    vs_out.proj_pos = p;
    gl_Position = p;
}

