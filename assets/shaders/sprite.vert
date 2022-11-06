uniform int camera_id;
uniform vec3 world_pos;
uniform float width;
uniform float height;

struct Sprite {
    vec3 world_pos;
    float world_width;
    float world_height;

    vec2 tex_pos;
    float tex_width;
    float tex_height;
};

struct Camera {
    vec3 world_pos;
    mat4 view_mat;
    mat4 proj_mat;
};

layout (std140, binding=CAMERA_BINDING_IDX) uniform Cameras {
    Camera cameras[MAX_N_CAMERAS];
    int n_cameras;
};

layout (std140, binding=SPRITE_BINDING_IDX) uniform Sprites {
    Sprite sprites[MAX_N_SPRITES];
    int n_sprites;
};

out VertexData {
    vec4 proj_pos;
    vec2 tex_pos;
} vertex_data;


void main() {
    // mat4 view_mat = cameras[camera_id].view_mat;
    // mat4 proj_mat = cameras[camera_id].proj_mat;

    // vec4 m = vec4(model_pos, 1.0);
    // vec4 w = world_mat * m; 
    // vec4 p = proj_mat * view_mat * w; 

    // vertex_data.model_pos = m;
    // vertex_data.world_pos = w;
    // vertex_data.proj_pos = p;
    // gl_Position = p;
}
