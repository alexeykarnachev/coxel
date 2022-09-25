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
    cam->far = 10000.0f;
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
    Vec3 z = vec3_norm(&view_dir);

    Vec3 x = vec3_cross(&z, &cam->up);
    x = vec3_norm(&x);

    Vec3 y = vec3_cross(&x, &z);
    y = vec3_norm(&y);

    z = vec3_negate(&z);

    Mat3 basis = mat3_from_rows(&x, &y, &z);

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

    cam->translation.data[0] += x.data[0] + y.data[0] - z.data[0];
    cam->translation.data[1] += x.data[1] + y.data[1] - z.data[1];
    cam->translation.data[2] += x.data[2] + y.data[2] - z.data[2];
}

void cam_rotate(Camera* cam, float pitch, float yaw) {
    // TODO: mod by PI
    cam->rotation.data[0] += pitch;
    cam->rotation.data[1] += yaw;
}

Mat4 cam_get_view_mat(Camera* cam) {
    Mat3 basis = cam_get_basis_mat(cam);
    Vec3 x = mat3_get_row(&basis, 0);
    Vec3 y = mat3_get_row(&basis, 1);
    Vec3 z = mat3_get_row(&basis, 2);

    Vec3 pos = { {
        cam->pos.data[0] + cam->translation.data[0],
        cam->pos.data[1] + cam->translation.data[1],
        cam->pos.data[2] + cam->translation.data[2]
    } };

    Vec4 row0 = vec3_append(&x, -vec3_dot(&x, &pos)); 
    Vec4 row1 = vec3_append(&y, -vec3_dot(&y, &pos));
    Vec4 row2 = vec3_append(&z, -vec3_dot(&z, &pos));
    Vec4 row3 = {0.0f, 0.0f, 0.0f, 1.0f};

    Mat4 view = mat4_from_rows(&row0, &row1, &row2, &row3);

    return view;
}

Mat4 cam_get_perspective_projection_mat(Camera* cam) {
    float f = 1.0f / tan(cam->fov / 2.0f);
    float rng_inv = 1.0f / (cam->near - cam->far);
    
    Mat4 proj = {
        {
            f / cam->aspect, 0.0f, 0.0f, 0.0f,
            0.0f, f, 0.0f, 0.0f,
            0.0f, 0.0f, (cam->near + cam->far) * rng_inv, cam->near * cam->far * rng_inv * 2.0f,
            0.0f, 0.0f, -1.0f, 0.0f
        }
    };

    return proj;
}

