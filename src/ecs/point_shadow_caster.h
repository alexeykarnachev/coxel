typedef struct PointShadowCaster {
    float near_plane;
    float far_plane;
    float min_n_samples;
    float max_n_samples;
    float disk_radius;
    float bias_min;
    float bias_max;
} PointShadowCaster;

PointShadowCaster _POINT_SHADOW_CASTERS_ARENA[1 << 14];
size_t _N_POINT_SHADOW_CASTERS = 0;


PointShadowCaster* point_shadow_caster_create(
    float near_plane,
    float far_plane,
    float min_n_samples,
    float max_n_samples,
    float disk_radius,
    float bias_min,
    float bias_max
) {
    PointShadowCaster* p =
        &_POINT_SHADOW_CASTERS_ARENA[_N_POINT_SHADOW_CASTERS++];

    p->near_plane = near_plane;
    p->far_plane = far_plane;
    p->min_n_samples = min_n_samples;
    p->max_n_samples = max_n_samples;
    p->disk_radius = disk_radius;
    p->bias_min = bias_min;
    p->bias_max = bias_max;

    return p;
}

PointShadowCaster* point_shadow_caster_create_default() {
    return point_shadow_caster_create(
        DEFAULT_POINT_SHADOW_NEAR_PLANE,
        DEFAULT_POINT_SHADOW_FAR_PLANE,
        DEFAULT_POINT_SHADOW_MIN_N_SAMPLES,
        DEFAULT_POINT_SHADOW_MAX_N_SAMPLES,
        DEFAULT_POINT_SHADOW_DISK_RADIUS,
        DEFAULT_POINT_SHADOW_BIAS_MIN,
        DEFAULT_POINT_SHADOW_BIAS_MAX
    );
}

