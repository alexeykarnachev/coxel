typedef struct CameraMouseControllerArgs {
    float side_sens;
    float straight_sens;
    float rotation_sens;
} CameraMouseControllerArgs;


Mat3 _get_basis_mat(Camera* cam, Transformation* t) {
    Mat3 rotation = mat3_rotation(
        t->rotation.data[0], t->rotation.data[1], 0.0f);
    Vec3 view_dir = mat3_vec3_mul(&rotation, &cam->view_dir);
    Mat3 basis = get_basis_mat(&view_dir, &cam->up);
    return basis;
}
    
void _translate(
    Camera* cam,
    Transformation* t,
    float dx,
    float dy,
    float dz
) {
    if (fabs(dx) + fabs(dy) + fabs(dz) < EPS) {
        return;
    }
    Mat3 basis = _get_basis_mat(cam, t);
    Vec3 x = mat3_get_row(&basis, 0);
    Vec3 y = mat3_get_row(&basis, 1);
    Vec3 z = mat3_get_row(&basis, 2);

    x = vec3_scale(&x, -dx);
    y = vec3_scale(&y, -dy);
    z = vec3_scale(&z, dz);
    
    t->position.data[0] += x.data[0] + y.data[0] + z.data[0];
    t->position.data[1] += x.data[1] + y.data[1] + z.data[1];
    t->position.data[2] += x.data[2] + y.data[2] + z.data[2];
}

void _rotate(Transformation* t, float pitch, float yaw) {
    // TODO: mod by PI
    t->rotation.data[0] += pitch;
    t->rotation.data[1] += yaw;
}

void _set_aspect(Camera* cam, float aspect) {
    cam->aspect = aspect;
}

void _camera_mouse_controller_update(size_t entity, void* args_p) {
    CameraMouseControllerArgs* args = (CameraMouseControllerArgs*)(args_p);
    Camera* cam = (Camera*)COMPONENTS[CAMERA_T][entity];
    Transformation* t = (Transformation*)COMPONENTS[TRANSFORMATION_T][entity];

    if (INPUT.shift_pressed && INPUT.mouse_middle_pressed) {
        _translate(
            cam,
            t,
            INPUT.cursor_dx * args->side_sens,
            INPUT.cursor_dy * args->side_sens,
            0.0
        );
    } else if (INPUT.mouse_middle_pressed) {
        _rotate(
            t,
            -INPUT.cursor_dy * args->rotation_sens,
            INPUT.cursor_dx * args->rotation_sens
        );
    } else if (fabs(INPUT.scroll_dy) > EPS) {
        _translate(
            cam,
            t,
            0.0f,
            0.0f,
            INPUT.scroll_dy * args->straight_sens
        );
    }

    float aspect = INPUT.window_width / INPUT.window_height;
    if (fabs(aspect - cam->aspect) > EPS) {
        _set_aspect(cam, aspect);
    }
}

CameraMouseControllerArgs camera_mouse_controller_create_default_args() {
    float side_sens = 30.0;
    float straight_sens = 3.0;
    float rotation_sens = 2.0;
    CameraMouseControllerArgs args = {
        side_sens, straight_sens, rotation_sens};
    return args;
}

Script* camera_mouse_controller_create_script(
    CameraMouseControllerArgs* args
) {
    return script_create(_camera_mouse_controller_update, args);
}
