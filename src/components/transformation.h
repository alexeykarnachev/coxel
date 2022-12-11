#pragma once
#include "../defaults.h"
#include "../la.h"

typedef struct Transformation {
    Vec3 scale;
    Mat3 rotation_mat;
    Vec3 translation;
} Transformation;

Transformation* transformation_create(
    Vec3 scale, Vec3 rotation, Vec3 translation
);
void transformation_create_from_model_mat(
    Transformation* t, Mat4* model_mat
);
Transformation* transformation_create_default();
Mat4 transformation_get_model_mat(Transformation* t);
void transformation_print(Transformation* t);
