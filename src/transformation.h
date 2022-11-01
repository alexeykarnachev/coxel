typedef struct Transformation {
    Vec3 scale;
    Vec3 rotation;
    Vec3 translation;
} Transformation;


Mat4 transformation_get_world_mat(Transformation* t) {
    return get_world_mat(&t->scale, &t->rotation, &t->translation);
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

void transformation_set_translation(Transformation* t, float x, float y, float z) {
    t->translation.data[0] = x;
    t->translation.data[1] = y;
    t->translation.data[2] = z;
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
    transformation_set_translation(
        t, t->translation.data[0] + dx, t->translation.data[1] + dy, t->translation.data[2] + dz);
}

