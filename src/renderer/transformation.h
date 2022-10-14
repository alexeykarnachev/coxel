typedef struct Transformation {
    Mat4 world_mat = mat4_identity();

    Vec3 scale = vec3_ones();
    Vec3 rotation = vec3_zeros();
    Vec3 translation = vec3_zeros();
} Transformation;

void transformation_create(Transformation* t, Vec3 scale, Vec3 rotation, Vec3 translation) {
    clear_struct(t);

    t->scale = scale;
    t->rotation = rotation;
    t->translation = translation;
    t->world_mat = get_world_mat(t->scale, t->rotation, t->translation);
}

void transformation_set_scale(Transformation* t, float x, float y, float z) {
    t->scale.data[0] = x;
    t->scale.data[1] = y;
    t->scale.data[2] = z;
    t->world_mat = get_world_mat(t->scale, t->rotation, t->translation);
}

void transformation_set_rotation(Transformation* t, float x, float y, float z) {
    t->rotation.data[0] = x;
    t->rotation.data[1] = y;
    t->rotation.data[2] = z;
    t->world_mat = get_world_mat(t->scale, t->rotation, t->translation);
}

void transformation_set_translation(Transformation* transformation, float x, float y, float z) {
    t->translation.data[0] = x;
    t->translation.data[1] = y;
    t->translation.data[2] = z;
    t->world_mat = get_world_mat(t->scale, t->rotation, t->translation);
}
