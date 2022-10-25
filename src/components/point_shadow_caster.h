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


bool point_shadow_caster_create_fbo(GLuint* fbo, GLuint* tex, size_t size, size_t n_casters) {
    glGenFramebuffers(1, fbo);
    glGenTextures(1, tex);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, *tex);
    glTexImage3D(
        GL_TEXTURE_CUBE_MAP_ARRAY,
        0,
        GL_DEPTH_COMPONENT,
        size,
        size,
        n_casters * 6,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        NULL
    );

    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_GREATER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *tex, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    check_framebuffer(false);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

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
        point_shadow_caster->view_proj_mats[i] = mat4_mat4_mul(&proj_mat, &view_mats[i]);
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
