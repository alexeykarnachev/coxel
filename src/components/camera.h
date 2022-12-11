#pragma once
#include "../la.h"
#include "transformation.h"

typedef struct Camera {
    float fov;
    float near;
    float far;
    float aspect;

    Vec3 view_dir;
    Vec3 up;
} Camera;

Camera* camera_create(float fov, float near, float far, float aspect);
Camera* camera_create_default();
Mat4 camera_get_view_mat(Camera* cam, Transformation* t);
Mat4 camera_get_perspective_projection_mat(Camera* cam);
float camera_is_codirected(Camera* cam, Transformation* t, Vec3* v);
