typedef struct Camera {
    float fov;
    float near;
    float far;
    float aspect;
    Vec3 rotation;
    Vec3 translation;

    Vec3 up;
    Vec3 view_dir;
    Vec3 pos;
} Camera;

bool cam_create(Camera* cam) {
    Vec3 rotation = { {0.0f, 0.0f, 0.0f} };
    Vec3 translation = { {0.0f, 0.0f, 0.0f} };
    Vec3 up = { {0.0, 1.0, 0.0} };
    Vec3 view_dir = { {0.0, 0.0, -1.0} };
    Vec3 pos = { {0.0f, 0.0f, 0.0f} };
    
    cam->fov = 45.0f;
    cam->near = 0.1f;
    cam->far = 1000000.0f;
    cam->aspect = 1.777f;
    cam->rotation = rotation;
    cam->translation = translation;
    cam->up = up;
    cam->view_dir = view_dir;
    cam->pos = pos;
}

static Mat3 cam_get_basis_mat(Camera* cam) {
    Mat3 rotation = mat3_rotation(cam->rotation.data[0], cam->rotation.data[1], 0.0f);
    Vec3 view_dir = mat3_vec3_mul(&rotation, &cam->view_dir);
    Mat3 basis = get_basis_mat(&view_dir, &cam->up);
    return basis;
}

void cam_translate(Camera* cam, float xd, float yd, float zd) {
    Mat3 basis = cam_get_basis_mat(cam);

    Vec3 x = mat3_get_row(&basis, 0);
    x = vec3_scale(&x, xd);

    Vec3 y = mat3_get_row(&basis, 1);
    y = vec3_scale(&y, yd);

    Vec3 z = mat3_get_row(&basis, 2);
    z = vec3_scale(&z, zd);

    cam->translation.data[0] += x.data[0] + y.data[0] + z.data[0];
    cam->translation.data[1] += x.data[1] + y.data[1] + z.data[1];
    cam->translation.data[2] += x.data[2] + y.data[2] + z.data[2];
}

void cam_rotate(Camera* cam, float pitch, float yaw) {
    // TODO: mod by PI
    cam->rotation.data[0] += pitch;
    cam->rotation.data[1] += yaw;
}

Mat4 cam_get_view_mat(Camera* cam) {
    Vec3 pos = { {
        cam->pos.data[0] + cam->translation.data[0],
        cam->pos.data[1] + cam->translation.data[1],
        cam->pos.data[2] + cam->translation.data[2]
    } };

    Mat3 rotation = mat3_rotation(cam->rotation.data[0], cam->rotation.data[1], 0.0f);
    Vec3 view_dir = mat3_vec3_mul(&rotation, &cam->view_dir);
    Mat4 view = get_view_mat(&view_dir, &cam->up, &pos);
    return view;
}

Mat4 cam_get_perspective_projection_mat(Camera* cam) {
    return get_perspective_projection_mat(cam->fov, cam->near, cam->far, cam->aspect);
}

