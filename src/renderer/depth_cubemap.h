typedef struct DepthCubemap {
    float view_proj_mats[96];
    Vec3 world_pos;
    size_t size;

    GLuint tex;
    GLuint fbo;
} DepthCubemap;

void depth_cubemap_create(DepthCubemap* depth_cubemap, size_t size);
void depth_cubemap_set_view(DepthCubemap* depth_cubemap, Vec3 world_pos, float min_dist, float max_dist);


void depth_cubemap_create(DepthCubemap* depth_cubemap, size_t size) {
    clear_struct(depth_cubemap);

    depth_cubemap->size = size;
    texture_create_depth_cubemap(&depth_cubemap->tex, size);
    buffer_create_depth_cubemap(&depth_cubemap->fbo, &depth_cubemap->tex);
}

void depth_cubemap_set_view(DepthCubemap* depth_cubemap, Vec3 world_pos, float min_dist, float max_dist) {
    Mat4 proj_mat = get_perspective_projection_mat(deg2rad(90.0), min_dist, max_dist, 1.0);

    Mat4 view_mats[6];
    Mat4 view_proj_mats[6];
    view_mats[0] = get_view_mat(&vec3_pos_x, &vec3_neg_y, &world_pos);
    view_mats[1] = get_view_mat(&vec3_neg_x, &vec3_neg_y, &world_pos);
    view_mats[2] = get_view_mat(&vec3_pos_y, &vec3_pos_z, &world_pos);
    view_mats[3] = get_view_mat(&vec3_neg_y, &vec3_neg_z, &world_pos);
    view_mats[4] = get_view_mat(&vec3_pos_z, &vec3_neg_y, &world_pos);
    view_mats[5] = get_view_mat(&vec3_neg_z, &vec3_neg_y, &world_pos);
    for (size_t i = 0; i < 6; ++i) {
        view_proj_mats[i] = mat4_mat4_mul(&proj_mat, &view_mats[i]);
    }
    mat4_pack(depth_cubemap->view_proj_mats, view_proj_mats, 6);
    depth_cubemap->world_pos = world_pos;
}
