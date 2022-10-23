typedef struct DepthCubemap {
    float view_proj_mats[96 * MAX_N_POINT_SHADOWS];
    float world_pos[3 * MAX_N_POINT_SHADOWS];
    size_t size;

    GLuint tex;
    GLuint fbo;
} DepthCubemap;

bool depth_cubemap_create(DepthCubemap* depth_cubemap, size_t size);
void depth_cubemap_set_views(
    DepthCubemap* depth_cubemap,
    Vec3 view_world_pos[],
    size_t n_views,
    float min_dist,
    float max_dist
);


bool depth_cubemap_create(DepthCubemap* depth_cubemap, size_t size) {
    clear_struct(depth_cubemap);
    depth_cubemap->size = size;
    return buffer_create_cube_shadowmap(&depth_cubemap->fbo, &depth_cubemap->tex, size, MAX_N_POINT_SHADOWS);
}

void depth_cubemap_set_views(
    DepthCubemap* depth_cubemap,
    Vec3 view_world_pos[],
    size_t n_views,
    float min_dist,
    float max_dist
) {
}
