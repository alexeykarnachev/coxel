typedef struct Transformation {
    Vec3 scale;
    Vec3 rotation;
    Vec3 position;
} Transformation;

Transformation _TRANSFORMATIONS_ARENA[1 << 14];
size_t _N_TRANSFORMATIONS = 0;


Transformation* transformation_create(
    Vec3 scale,
    Vec3 rotation,
    Vec3 position
) {
    Transformation* t = &_TRANSFORMATIONS_ARENA[_N_TRANSFORMATIONS++];

    t->scale = scale;
    t->rotation = rotation;
    t->position = position;

    return t;
}

Transformation* transformation_create_default() {
    return transformation_create(vec3_ones(), vec3_zeros(), vec3_zeros());
}

Mat4 transformation_get_world_mat(Transformation* t) {
    return get_world_mat(&t->scale, &t->rotation, &t->position);
}

void transformation_set_scale(Transformation* t, float x, float y, float z) {
    t->scale.data[0] = x;
    t->scale.data[1] = y;
    t->scale.data[2] = z;
}

void transformation_set_rotation(Transformation* t, float x, float y, float z) {
    t->rotation.data[0] = x;
    t->rotation.data[1] = y;
    t->rotation.data[2] = z;
}

void transformation_set_position(Transformation* t, float x, float y, float z) {
    t->position.data[0] = x;
    t->position.data[1] = y;
    t->position.data[2] = z;
}

void transformation_scale(Transformation* t, float dx, float dy, float dz) {
    transformation_set_scale(
        t, t->scale.data[0] + dx, t->scale.data[1] + dy, t->scale.data[2] + dz);
}

void transformation_rotate(Transformation* t, float dx, float dy, float dz) {
    transformation_set_rotation(
        t, t->rotation.data[0] + dx, t->rotation.data[1] + dy, t->rotation.data[2] + dz);
}

void transformation_translate(Transformation* t, float dx, float dy, float dz) {
    transformation_set_position(
        t, t->positions.data[0] + dx, t->position.data[1] + dy, t->position.data[2] + dz);
}

