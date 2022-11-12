typedef struct Camera {
    float fov;
    float near;
    float far;
    float aspect;

    Vec3 view_dir;
    Vec3 up;
} Camera;

Camera _CAMERAS_ARENA[128];
size_t _N_CAMERAS = 0;


Camera* camera_create(float fov, float near, float far, float aspect) {
    Camera* c = &_CAMERAS_ARENA[_N_CAMERAS++];

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
    Mat3 rotation = mat3_rotation(
        t->rotation.data[0], t->rotation.data[1], 0.0f);
    Vec3 view_dir = mat3_vec3_mul(&rotation, &cam->view_dir);
    return get_view_mat(&view_dir, &cam->up, &t->position);
}

Mat4 camera_get_perspective_projection_mat(Camera* camera) {
    return get_perspective_projection_mat(
        camera->fov, camera->near, camera->far, camera->aspect);
}

