#include "camera_mouse_controller.h"
#include "../components/script.h"
#include "../components/camera.h"
#include "../components/transformation.h"
#include "../window.h"
#include "../ecs.h"
#include "../la.h"


static void translate(
    Camera* cam,
    Transformation* t,
    float dx,
    float dy,
    float dz
) {
    static float eps = 0.00001;
    if (fabs(dx) + fabs(dy) + fabs(dz) < eps) {
        return;
    }

    Vec3 view_dir = mat3_vec3_mul(&t->rotation_mat, &cam->view_dir);
    Mat3 basis = get_basis_mat(&view_dir, &cam->up);

    Vec3 x = mat3_get_row(&basis, 0);
    Vec3 y = mat3_get_row(&basis, 1);
    Vec3 z = mat3_get_row(&basis, 2);

    x = vec3_scale(&x, -dx);
    y = vec3_scale(&y, -dy);
    z = vec3_scale(&z, dz);
    
    t->translation.data[0] += x.data[0] + y.data[0] + z.data[0];
    t->translation.data[1] += x.data[1] + y.data[1] + z.data[1];
    t->translation.data[2] += x.data[2] + y.data[2] + z.data[2];
}

static void rotate(Transformation* t, float pitch, float yaw) {
    Vec3 rotation = vec3(pitch, yaw, 0.0);
    Mat3 rotation_mat = mat3_rotation(&rotation);
    t->rotation_mat = mat3_mat3_mul(&t->rotation_mat, &rotation_mat);
}

static void camera_mouse_controller_update(size_t entity, void* args_p) {
    static float eps = 0.00001;
    CameraMouseControllerArgs* args = (CameraMouseControllerArgs*)(args_p);
    Camera* cam = (Camera*)COMPONENTS[CAMERA_T][entity];
    Transformation* t = (Transformation*)COMPONENTS[TRANSFORMATION_T][entity];

    if (INPUT.shift_pressed && INPUT.mouse_middle_pressed) {
        translate(
            cam,
            t,
            INPUT.cursor_dx * args->side_sens,
            INPUT.cursor_dy * args->side_sens,
            0.0
        );
    } else if (INPUT.mouse_middle_pressed) {
        rotate(
            t,
            -INPUT.cursor_dy * args->rotation_sens,
            INPUT.cursor_dx * args->rotation_sens
        );
    } else if (fabs(INPUT.scroll_dy) > eps) {
        translate(
            cam,
            t,
            0.0f,
            0.0f,
            INPUT.scroll_dy * args->straight_sens
        );
    }

    float aspect = INPUT.window_width / INPUT.window_height;
    if (fabs(aspect - cam->aspect) > eps) {
        cam->aspect = aspect;
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
    return script_create(camera_mouse_controller_update, args);
}

