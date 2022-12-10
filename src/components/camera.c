#include "camera.h"

static Camera CAMERAS_ARENA[128];
static size_t N_CAMERAS = 0;


Camera* camera_create(float fov, float near, float far, float aspect) {
    Camera* c = &CAMERAS_ARENA[N_CAMERAS++];

    c->fov = fov;
    c->near = near;
    c->far = far;
    c->aspect = aspect;

    // TODO: These should be not a part of a camera, but a part of tranformation.
    c->up = vec3(0.0, 1.0, 0.0);
    c->view_dir = vec3(0.0, 0.0, -1.0);

    return c;
}

Camera* camera_create_default() {
    return camera_create(
        DEFAULT_CAMERA_FOV,
        DEFAULT_CAMERA_NEAR,
        DEFAULT_CAMERA_FAR,
        DEFAULT_CAMERA_ASPECT
    );
}

// TODO: Decouple camera and transformation somehow. At least,
// reduce code duplication. The same operations (view_dir calculation)
// is applied in the camera controller script.
// IDEA: don't store view_dir and up in camera. It could be
// inferenced from the transformation.
// IDEA: this function is not a camera function, it's essentially
// a tranformation function!
Mat4 camera_get_view_mat(Camera* cam, Transformation* t) {
    Vec3 view_dir = mat3_vec3_mul(&t->rotation_mat, &cam->view_dir);
    return get_view_mat(&view_dir, &cam->up, &t->translation);
}

Mat4 camera_get_perspective_projection_mat(Camera* cam) {
    return get_perspective_projection_mat(
        cam->fov, cam->near, cam->far, cam->aspect);
}

float camera_is_codirected(Camera* cam, Transformation* t, Vec3* v) {
    Vec3 view_dir = mat3_vec3_mul(&t->rotation_mat, &cam->view_dir);
    float sign = sign(vec3_dot(&view_dir, v));
    if (fabs(sign) < EPS) {
        return 1.0;
    }
    return sign;
}

