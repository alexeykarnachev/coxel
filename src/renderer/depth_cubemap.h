typedef struct DepthCubemap {
    Mat4 proj_mat;
    Mat4 view_proj_mats[6];
    Vec3 origin_world_pos;
    float max_dist;
    float min_dist;
} DepthCubemap;

void depth_cubemap_create(DepthCubemap* depth_cubemap, float min_dist, float max_dist);
void depth_cubemap_set_origin_world_pos(DepthCubemap* depth_cubemap, Vec3 origin_world_pos);


void depth_cubemap_create(DepthCubemap* depth_cubemap, float min_dist, float max_dist) {
    clear_struct(depth_cubemap);

    depth_cubemap->min_dist = min_dist;
    depth_cubemap->max_dist = max_dist;
    depth_cubemap->proj_mat = get_perspective_projection_mat(deg2rad(90.0), min_dist, max_dist, 1.0);
}

void depth_cubemap_set_origin_world_pos(DepthCubemap* depth_cubemap, Vec3 origin_world_pos) {
    depth_cubemap->origin_world_pos = origin_world_pos;

    Mat4 view_mats[6];
    view_mats[0] = get_view_mat(&vec3_pos_x, &vec3_neg_y, &origin_world_pos);
    view_mats[1] = get_view_mat(&vec3_neg_x, &vec3_neg_y, &origin_world_pos);
    view_mats[2] = get_view_mat(&vec3_pos_y, &vec3_pos_z, &origin_world_pos);
    view_mats[3] = get_view_mat(&vec3_neg_y, &vec3_neg_z, &origin_world_pos);
    view_mats[4] = get_view_mat(&vec3_pos_z, &vec3_neg_y, &origin_world_pos);
    view_mats[5] = get_view_mat(&vec3_neg_z, &vec3_neg_y, &origin_world_pos);
    for (size_t i = 0; i < 6; ++i) {
        depth_cubemap->view_proj_mats[i] = mat4_mat4_mul(&depth_cubemap->proj_mat, &view_mats[i]);
    }
}
