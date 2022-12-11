#include "transformation.h"

static Transformation TRANSFORMATIONS_ARENA[1 << 14];
static size_t N_TRANSFORMATIONS = 0;

Transformation*
transformation_create(Vec3 scale, Vec3 rotation, Vec3 translation) {
    Transformation* t = &TRANSFORMATIONS_ARENA[N_TRANSFORMATIONS++];

    Mat3 rotation_mat = mat3_rotation(&rotation);
    t->scale = scale;
    t->rotation_mat = rotation_mat;
    t->translation = translation;

    return t;
}

void transformation_create_from_model_mat(Transformation* t, Mat4* model_mat) {
    t->scale = model_mat_extract_scale_vec(model_mat);
    t->rotation_mat = model_mat_extract_rotation_mat(model_mat);
    t->translation = model_mat_extract_translation_vec(model_mat);
}

Transformation* transformation_create_default() {
    return transformation_create(
        DEFAULT_TRANSFORMATION_SCALE,
        DEFAULT_TRANSFORMATION_ROTATION,
        DEFAULT_TRANSFORMATION_TRANSLATION
    );
}

Mat4 transformation_get_model_mat(Transformation* t) {
    return get_world_mat(&t->scale, &t->rotation_mat, &t->translation);
}

void transformation_print(Transformation* t) {
    printf("scale:\n");
    vec3_print(&t->scale);
    printf("translation:\n");
    vec3_print(&t->translation);
    printf("rotation_mat:\n");
    mat3_print(&t->rotation_mat);
    printf("\n");
}
