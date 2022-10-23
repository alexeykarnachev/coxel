#define _POINT_LIGHT_UBO_N_BYTES 48

typedef struct PointLight {
    Vec3 world_pos;
    Vec3 color;
    float energy;
    PointShadowCaster* point_shadow_caster;
} PointLight;

int POINT_LIGHT_UBO = -1;
PointLight _POINT_LIGHT_ARENA[MAX_N_POINT_LIGHTS];
size_t _POINT_LIGHT_ARENA_IDX = 0;


void point_light_pack(PointLight* point_light, float dst[]) {
    size_t size = sizeof(float);
    memcpy(&dst[0], point_light->world_pos.data, size * 3);
    memcpy(&dst[4], point_light->color.data, size * 3);
    dst[8] = point_light->energy;
}

PointLight* point_light_create(Vec3 world_pos, Vec3 color, float energy) {
    if (_POINT_LIGHT_ARENA_IDX == MAX_N_POINT_LIGHTS) {
        fprintf(stderr, "ERROR: max number of point lights is reached. Point light won't be created");
        return NULL;
    }
    PointLight* point_light = &_POINT_LIGHT_ARENA[_POINT_LIGHT_ARENA_IDX];

    point_light->world_pos = world_pos;
    point_light->color = color;
    point_light->energy = energy;

    static float data[_POINT_LIGHT_UBO_N_BYTES / 4];
    point_light_pack(point_light, data);

    if (POINT_LIGHT_UBO == -1) {
        glGenBuffers(1, &POINT_LIGHT_UBO);
        glBindBuffer(GL_UNIFORM_BUFFER, POINT_LIGHT_UBO);
        glBufferData(
            GL_UNIFORM_BUFFER,
            _POINT_LIGHT_UBO_N_BYTES * MAX_N_POINT_LIGHTS + 16,
            NULL,
            GL_DYNAMIC_DRAW
        );
    }

    glBindBuffer(GL_UNIFORM_BUFFER, POINT_LIGHT_UBO);
    glBufferSubData(
        GL_UNIFORM_BUFFER,
        _POINT_LIGHT_ARENA_IDX * _POINT_LIGHT_UBO_N_BYTES,
        _POINT_LIGHT_UBO_N_BYTES,
        data
    );
    _POINT_LIGHT_ARENA_IDX += 1;

    glBufferSubData(
        GL_UNIFORM_BUFFER,
        MAX_N_POINT_LIGHTS * _POINT_LIGHT_UBO_N_BYTES,
        16,
        &_POINT_LIGHT_ARENA_IDX
    );

    glBindBufferBase(GL_UNIFORM_BUFFER, POINT_LIGHT_BINDING_IDX, POINT_LIGHT_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return point_light;
}

void point_light_attach_point_shadow_caster(PointLight* point_light) {
    point_light->point_shadow_caster = point_shadow_caster_create(
        POINT_SHADOW_NEAR_PLANE,
        POINT_SHADOW_FAR_PLANE,
        POINT_SHADOW_MIN_N_SAMPLES,
        POINT_SHADOW_MAX_N_SAMPLES,
        POINT_SHADOW_DISK_RADIUS,
        POINT_SHADOW_BIAS_MIN,
        POINT_SHADOW_BIAS_MAX,
        point_light->world_pos
    );
}
