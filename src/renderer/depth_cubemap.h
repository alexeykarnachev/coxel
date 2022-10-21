#define MAX_N_VIEWS 4

typedef struct DepthCubemap {
    float view_proj_mats[96 * MAX_N_VIEWS];
    float world_pos[MAX_N_VIEWS];
    size_t size;

    GLuint tex;
    GLuint fbo;
} DepthCubemap;

void depth_cubemap_create(DepthCubemap* depth_cubemap, size_t size);
void depth_cubemap_set_views(
    DepthCubemap* depth_cubemap,
    Vec3 view_world_pos[],
    size_t n_views,
    float min_dist,
    float max_dist
);


void depth_cubemap_create(DepthCubemap* depth_cubemap, size_t size) {
    clear_struct(depth_cubemap);

    depth_cubemap->size = size;
    buffer_create_cube_shadowmap(&depth_cubemap->fbo, &depth_cubemap->tex, size, MAX_N_VIEWS);
}

void depth_cubemap_set_views(
    DepthCubemap* depth_cubemap,
    Vec3 view_world_pos[],
    size_t n_views,
    float min_dist,
    float max_dist
) {
    Mat4 proj_mat = get_perspective_projection_mat(deg2rad(90.0), min_dist, max_dist, 1.0);

    Mat4 view_mats[6];
    Mat4 view_proj_mats[6];
    for (size_t i = 0; i < min(n_views, MAX_N_VIEWS); ++i) {
        Vec3* world_pos = &(view_world_pos[i]);
        view_mats[0] = get_view_mat(&vec3_pos_x, &vec3_neg_y, world_pos);
        view_mats[1] = get_view_mat(&vec3_neg_x, &vec3_neg_y, world_pos);
        view_mats[2] = get_view_mat(&vec3_pos_y, &vec3_pos_z, world_pos);
        view_mats[3] = get_view_mat(&vec3_neg_y, &vec3_neg_z, world_pos);
        view_mats[4] = get_view_mat(&vec3_pos_z, &vec3_neg_y, world_pos);
        view_mats[5] = get_view_mat(&vec3_neg_z, &vec3_neg_y, world_pos);
        for (size_t i = 0; i < 6; ++i) {
            view_proj_mats[i] = mat4_mat4_mul(&proj_mat, &view_mats[i]);
        }
        mat4_pack(&(depth_cubemap->view_proj_mats[96 * i]), view_proj_mats, 6);
    }
    // vec3_pack(depth_cubemap->world_pos, view_world_pos, n_views);
}
