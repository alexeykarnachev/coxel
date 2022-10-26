typedef struct CameraMouseController {
    int32_t camera_gid;

    float side_sens;
    float straight_sens;
    float rotation_sens;

    void (*script)();
} CameraMouseController;

CameraMouseController camera_mouse_controller_create(
    float side_sens,
    float straight_sens,
    float rotation_sens
) {
    CameraMouseController controller;
    controller.side_sens = side_sens;
    controller.straight_sens = straight_sens;
    controller.rotation_sens = rotation_sens;
}


Mat3 _camera_get_basis_mat(CameraMouseController* c) {
    Camera* cam = c->camera;
    Mat3 rotation = mat3_rotation(cam->rotation.data[0], cam->rotation.data[1], 0.0f);
    Vec3 view_dir = mat3_vec3_mul(&rotation, &cam->view_dir);
    Mat3 basis = get_basis_mat(&view_dir, &cam->up);
    return basis;
}
    
void _camera_translate(CameraMouseController* c, float dx, float dy, float dz) {
    Camera* cam = c->camera;
    if (fabs(dx) + fabs(dy) + fabs(dz) < EPS) {
        return;
    }
    Mat3 basis = _camera_get_basis_mat(c);
    Vec3 x = mat3_get_row(&basis, 0);
    Vec3 y = mat3_get_row(&basis, 1);
    Vec3 z = mat3_get_row(&basis, 2);

    x = vec3_scale(&x, dx * c->side_sens);
    y = vec3_scale(&y, dy * c->side_sens);
    z = vec3_scale(&z, dz * c->straight_sens);
    
    cam->translation.data[0] += x.data[0] + y.data[0] + z.data[0];
    cam->translation.data[1] += x.data[1] + y.data[1] + z.data[1];
    cam->translation.data[2] += x.data[2] + y.data[2] + z.data[2];
}

void _camera_rotate(CameraMouseController* c, float pitch, float yaw) {
    Camera* cam = c->camera;
    // TODO: mod by PI
    cam->rotation.data[0] += pitch * c->rotation_sens;
    cam->rotation.data[1] += yaw * c->rotation_sens;
}

void _camera_set_aspect(CameraMouseController* c, float aspect) {
    c->camera->aspect = aspect;
}

void _camera_controller_update(CameraMouseController* c) {
    if (INPUT.mouse_middle_pressed) {
        _camera_translate(c, INPUT.cursor_dx, INPUT.cursor_dy, 0.0);
    } 

    if (INPUT.mouse_left_pressed) {
        _camera_rotate(c, INPUT.cursor_dy, -INPUT.cursor_dx);
    }

    _camera_translate(c, 0.0f, 0.0f, INPUT.scroll_dy);
    _camera_set_aspect(c, INPUT.window_width / INPUT.window_height);
}

