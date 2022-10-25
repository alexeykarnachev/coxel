# define _TRANSFORMATION_UBO_N_BYTES 64

typedef struct Transformation {
    Vec3 scale;
    Vec3 rotation;
    Vec3 translation;
} Transformation;

Transformation _TRANSFORMATION_ARENA[MAX_N_TRANSFORMATIONS];
size_t _TRANSFORMATION_ARENA_IDX = 0;
int _TRANSFORMATION_UBO = -1;


void _transformation_pack(Transformation* t, float dst[]) {
    Mat4 world_mat = get_world_mat(&t->scale, &t->rotation, &t->translation);
    mat4_transpose_pack(dst, &world_mat, 1);
}

void _transformation_create_ubo() {
    glGenBuffers(1, &_TRANSFORMATION_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, _TRANSFORMATION_UBO);
    glBufferData(
        GL_UNIFORM_BUFFER,
        _TRANSFORMATION_UBO_N_BYTES * MAX_N_TRANSFORMATIONS,
        NULL,
        GL_DYNAMIC_DRAW
    );

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, TRANSFORMATION_BINDING_IDX, _TRANSFORMATION_UBO);
}

void _transformation_update_ubo(size_t transformation_id) {
    if (_TRANSFORMATION_UBO == -1) {
        _transformation_create_ubo();
    }

    static float data[_TRANSFORMATION_UBO_N_BYTES / 4];
    _transformation_pack(&_TRANSFORMATION_ARENA[transformation_id], data);

    glBindBuffer(GL_UNIFORM_BUFFER, _TRANSFORMATION_UBO);
    glBufferSubData(
        GL_UNIFORM_BUFFER,
        transformation_id * _TRANSFORMATION_UBO_N_BYTES,
        _TRANSFORMATION_UBO_N_BYTES,
        data
    );
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

int transformation_create(Vec3 scale, Vec3 rotation, Vec3 translation) {
    if (_TRANSFORMATION_ARENA_IDX == MAX_N_TRANSFORMATIONS) {
        fprintf(stderr, "ERROR: max number of transformations is reached. Transformation won't be created");
        return -1;
    }
    Transformation* t = &_TRANSFORMATION_ARENA[_TRANSFORMATION_ARENA_IDX];

    t->scale = scale;
    t->rotation = rotation;
    t->translation = translation;

    _transformation_update_ubo(_TRANSFORMATION_ARENA_IDX);
    return _TRANSFORMATION_ARENA_IDX++;
}

void transformation_set_scale(size_t transformation_id, float x, float y, float z) {
    Transformation* t = &_TRANSFORMATION_ARENA[transformation_id];
    t->scale.data[0] = x;
    t->scale.data[1] = y;
    t->scale.data[2] = z;
}

void transformation_set_rotation(size_t transformation_id, float x, float y, float z) {
    Transformation* t = &_TRANSFORMATION_ARENA[transformation_id];
    t->rotation.data[0] = x;
    t->rotation.data[1] = y;
    t->rotation.data[2] = z;
}

void transformation_set_translation(size_t transformation_id, float x, float y, float z) {
    Transformation* t = &_TRANSFORMATION_ARENA[transformation_id];
    t->translation.data[0] = x;
    t->translation.data[1] = y;
    t->translation.data[2] = z;
}

void transformation_scale(size_t transformation_id, float dx, float dy, float dz) {
    Transformation* t = &_TRANSFORMATION_ARENA[transformation_id];
    transformation_set_scale(
        transformation_id, t->scale.data[0] + dx, t->scale.data[1] + dy, t->scale.data[2] + dz);
}

void transformation_rotate(size_t transformation_id, float dx, float dy, float dz) {
    Transformation* t = &_TRANSFORMATION_ARENA[transformation_id];
    transformation_set_rotation(
        transformation_id, t->rotation.data[0] + dx, t->rotation.data[1] + dy, t->rotation.data[2] + dz);
}

void transformation_translate(size_t transformation_id, float dx, float dy, float dz) {
    Transformation* t = &_TRANSFORMATION_ARENA[transformation_id];
    transformation_set_translation(
        transformation_id, t->translation.data[0] + dx, t->translation.data[1] + dy, t->translation.data[2] + dz);
}

