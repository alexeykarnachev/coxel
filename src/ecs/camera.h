typedef struct Camera {
    float fov;
    float near;
    float far;
    float aspect;
} Camera;

Camera _CAMERAS_ARENA[128];
size_t _N_CAMERAS = 0;


Camera* camera_create(float fov_deg, float near, float far, float aspect) {
    Camera* c = &_CAMERAS_ARENA[_N_CAMERAS++];

    c->fov = deg2rad(fov_deg);
    c->near = near;
    c->far = far;
    c->aspect = aspect;

    return c;
}

Camera* camera_create_default() {
    return camera_create(45.0, 0.1, 100.0, 16.0 / 9.0);
}

Mat4 camera_get_view_mat(Transformation* transformation) {
    Vec3 up = {{0.0, 1.0, 0.0}};
    Vec3 view_dir = {{0.0, 0.0, -1.0}};
    return get_view_mat(&view_dir, &up, transformation->position);
}

Mat4 camera_get_perspective_projection_mat(Camera* camera) {
    return get_perspective_projection_mat(
        camera->fov, camera->near, camera->far, camera->aspect);
}

