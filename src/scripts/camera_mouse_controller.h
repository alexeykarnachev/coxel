typedef struct CameraMouseControllerArgs {
    int32_t camera_gid;

    float side_sens;
    float straight_sens;
    float rotation_sens;
} CameraMouseControllerArgs;


Mat3 _camera_get_basis_mat(Camera* cam) {
    Mat3 rotation = mat3_rotation(cam->rotation.data[0], cam->rotation.data[1], 0.0f);
    Vec3 view_dir = mat3_vec3_mul(&rotation, &cam->view_dir);
    Mat3 basis = get_basis_mat(&view_dir, &cam->up);
    return basis;
}
    
void _camera_translate(Camera* cam, float dx, float dy, float dz) {
    if (fabs(dx) + fabs(dy) + fabs(dz) < EPS) {
        return;
    }
    Mat3 basis = _camera_get_basis_mat(cam);
    Vec3 x = mat3_get_row(&basis, 0);
    Vec3 y = mat3_get_row(&basis, 1);
    Vec3 z = mat3_get_row(&basis, 2);

    x = vec3_scale(&x, dx);
    y = vec3_scale(&y, dy);
    z = vec3_scale(&z, dz);
    
    cam->translation.data[0] += x.data[0] + y.data[0] + z.data[0];
    cam->translation.data[1] += x.data[1] + y.data[1] + z.data[1];
    cam->translation.data[2] += x.data[2] + y.data[2] + z.data[2];
}

void _camera_rotate(Camera* cam, float pitch, float yaw) {
    // TODO: mod by PI
    cam->rotation.data[0] += pitch;
    cam->rotation.data[1] += yaw;
}

void _camera_set_aspect(Camera* cam, float aspect) {
    cam->aspect = aspect;
}

void _camera_mouse_controller_update(void* args_p) {
    CameraMouseControllerArgs* args = (CameraMouseControllerArgs*)(args_p);
    Camera* cam = (Camera*)((&SCENE_COMPONENTS[args->camera_gid])->ptr);
    bool needs_update = false;
    if (INPUT.mouse_middle_pressed) {
        needs_update = true;
        _camera_translate(
            cam, INPUT.cursor_dx * args->side_sens, INPUT.cursor_dy * args->side_sens, 0.0);
    } 

    if (INPUT.mouse_left_pressed) {
        needs_update = true;
        _camera_rotate(
            cam, INPUT.cursor_dy * args->rotation_sens, -INPUT.cursor_dx * args->rotation_sens);
    }

    if (fabs(INPUT.scroll_dy) > EPS) {
        needs_update = true;
        _camera_translate(cam, 0.0f, 0.0f, INPUT.scroll_dy * args->straight_sens);
    }

    float aspect = INPUT.window_width / INPUT.window_height;
    if (fabs(aspect - cam->aspect) > EPS) {
        needs_update = true;
        _camera_set_aspect(cam, aspect);
    }

    if (needs_update) {
        scene_update_component(args->camera_gid);
    }
}

Script camera_mouse_controller_create_script(
    CameraMouseControllerArgs* args
) {
    Script script;
    script.update = &_camera_mouse_controller_update;
    script.args = args; 
    return script;
}
