#define POINT_SHADOW_CASTER_PACK_SIZE 432

typedef struct PointShadowCaster {
    float near_plane;
    float far_plane;
    float min_n_samples;
    float max_n_samples;
    float disk_radius;
    float bias_min;
    float bias_max;
    Vec3 world_pos;
    Mat4 view_proj_mats[6];
} PointShadowCaster;


PointShadowCaster point_shadow_caster_create(
    float near_plane,
    float far_plane,
    float min_n_samples,
    float max_n_samples,
    float disk_radius,
    float bias_min,
    float bias_max,
    Vec3 world_pos
) {
    PointShadowCaster point_shadow_caster;

    point_shadow_caster.near_plane = near_plane;
    point_shadow_caster.far_plane = far_plane;
    point_shadow_caster.min_n_samples = min_n_samples;
    point_shadow_caster.max_n_samples = max_n_samples;
    point_shadow_caster.disk_radius = disk_radius;
    point_shadow_caster.bias_min = bias_min;
    point_shadow_caster.bias_max = bias_max;
    point_shadow_caster.world_pos = world_pos;

    Mat4 proj_mat = get_perspective_projection_mat(deg2rad(90.0), near_plane, far_plane, 1.0);

    Mat4 view_mats[6];
    view_mats[0] = get_view_mat(&vec3_pos_x, &vec3_neg_y, &world_pos);
    view_mats[1] = get_view_mat(&vec3_neg_x, &vec3_neg_y, &world_pos);
    view_mats[2] = get_view_mat(&vec3_pos_y, &vec3_pos_z, &world_pos);
    view_mats[3] = get_view_mat(&vec3_neg_y, &vec3_neg_z, &world_pos);
    view_mats[4] = get_view_mat(&vec3_pos_z, &vec3_neg_y, &world_pos);
    view_mats[5] = get_view_mat(&vec3_neg_z, &vec3_neg_y, &world_pos);
    for (size_t i = 0; i < 6; ++i) {
        point_shadow_caster.view_proj_mats[i] = mat4_mat4_mul(&proj_mat, &view_mats[i]);
    }

    return point_shadow_caster;
}

void point_shadow_caster_pack(PointShadowCaster* point_shadow_caster, float dst[]) {
    dst[0] = point_shadow_caster->near_plane;
    dst[1] = point_shadow_caster->far_plane;
    dst[2] = point_shadow_caster->min_n_samples;
    dst[3] = point_shadow_caster->max_n_samples;
    dst[4] = point_shadow_caster->disk_radius;
    dst[5] = point_shadow_caster->bias_min;
    dst[6] = point_shadow_caster->bias_max;
    memcpy(&dst[8], point_shadow_caster->world_pos.data, sizeof(float) * 3);
    mat4_transpose_pack(&dst[12], point_shadow_caster->view_proj_mats, 6);
}
