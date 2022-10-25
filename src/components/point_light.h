#define _POINT_LIGHT_UBO_N_BYTES 48

typedef struct PointLight {
    Vec3 world_pos;
    Vec3 color;
    float energy;
} PointLight;

PointLight _POINT_LIGHT_ARENA[MAX_N_POINT_LIGHTS];
size_t N_POINT_LIGHTS = 0;
int _POINT_LIGHT_UBO = -1;


PointLight* point_light_get(size_t id) {
    return &_POINT_LIGHT_ARENA[id - POINT_LIGHT_START_ID];
}

void _point_light_pack(PointLight* point_light, float dst[]) {
    size_t size = sizeof(float);
    memcpy(&dst[0], point_light->world_pos.data, size * 3);
    memcpy(&dst[4], point_light->color.data, size * 3);
    dst[8] = point_light->energy;
}

void _point_light_create_ubo() {
    glGenBuffers(1, &_POINT_LIGHT_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, _POINT_LIGHT_UBO);
    glBufferData(
        GL_UNIFORM_BUFFER,
        _POINT_LIGHT_UBO_N_BYTES * MAX_N_POINT_LIGHTS + 16,
        NULL,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, POINT_LIGHT_BINDING_IDX, _POINT_LIGHT_UBO);
}

void _point_light_update_ubo(size_t point_light_id) {
    if (_POINT_LIGHT_UBO == -1) {
        _point_light_create_ubo();
    }

    static float data[_POINT_LIGHT_UBO_N_BYTES / 4];
    _point_light_pack(point_light_get(point_light_id), data);

    glBindBuffer(GL_UNIFORM_BUFFER, _POINT_LIGHT_UBO);
    glBufferSubData(
        GL_UNIFORM_BUFFER,
        point_light_id * _POINT_LIGHT_UBO_N_BYTES,
        _POINT_LIGHT_UBO_N_BYTES,
        data
    );

    size_t n_point_lights = N_POINT_LIGHTS + 1;
    glBufferSubData(
        GL_UNIFORM_BUFFER,
        MAX_N_POINT_LIGHTS * _POINT_LIGHT_UBO_N_BYTES,
        16,
        &n_point_lights
    );

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

int point_light_create(Vec3 world_pos, Vec3 color, float energy) {
    if (N_POINT_LIGHTS == MAX_N_POINT_LIGHTS) {
        fprintf(stderr, "ERROR: max number of point lights is reached. Point light won't be created");
        return -1;
    }
    size_t id = POINT_LIGHT_START_ID + (N_POINT_LIGHTS++);
    PointLight* point_light = point_light_get(id);

    point_light->world_pos = world_pos;
    point_light->color = color;
    point_light->energy = energy;

    _point_light_update_ubo(id);
    return id;
}

