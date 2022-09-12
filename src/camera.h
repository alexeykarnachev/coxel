static Vec3 VIEW_DIR = { {0.0f, 0.0f, -1.0f} };
static Vec3 INITIAL_VIEW_DIR = { {0.0f, 0.0f, -1.0f} };
static Vec3 POSITION = { {0.0f, 0.0f, 2.0f} };
static Vec3 UP = { {0.0f, 1.0f, 0.0f} };

static float FOV = 45.0f;
static float NEAR = 0.1f;
static float FAR = 100.0f;
static float ASPECT_RATIO = 4.0f / 3.0f;
static float PITCH = 0.0f;
static float YAW = 0.0f;

static float ROTATE_SENS = 1.0f;
static float MOVE_SENS = 5.0f;

static Mat3 cam_get_basis() {
    Vec3 z = vec3_norm(&VIEW_DIR);

    Vec3 x = vec3_cross(&z, &UP);
    x = vec3_norm(&x);

    Vec3 y = vec3_cross(&x, &z);
    y = vec3_norm(&y);

    z = vec3_negate(&z);

    Mat3 basis = mat3_from_rows(&x, &y, &z);

    return basis;
}

void cam_move(float xd, float yd) {
    Mat3 basis = cam_get_basis();

    Vec3 x = mat3_get_row(&basis, 0);
    x = vec3_scale(&x, xd * MOVE_SENS);

    Vec3 y = mat3_get_row(&basis, 1);
    y = vec3_scale(&y, yd * MOVE_SENS);

    POSITION.data[0] += x.data[0] + y.data[0];
    POSITION.data[1] += x.data[1] + y.data[1];
    POSITION.data[2] += x.data[2] + y.data[2];
}

void cam_rotate(float pitch, float yaw) {
    // TODO: mod by PI
    PITCH = ROTATE_SENS * (PITCH + pitch);
    YAW = ROTATE_SENS * (YAW + yaw);

    Mat3 r = mat3_rotation(PITCH, YAW, 0.0f);
    VIEW_DIR = mat3_vecmul(&r, &INITIAL_VIEW_DIR);
}

Mat4 cam_get_view() {
    Mat3 basis = cam_get_basis();
    Vec3 x = mat3_get_row(&basis, 0);
    Vec3 y = mat3_get_row(&basis, 1);
    Vec3 z = mat3_get_row(&basis, 2);

    Vec4 row0 = vec3_append(&x, -vec3_dot(&x, &POSITION)); 
    Vec4 row1 = vec3_append(&y, -vec3_dot(&y, &POSITION));
    Vec4 row2 = vec3_append(&z, -vec3_dot(&z, &POSITION));
    Vec4 row3 = {0.0f, 0.0f, 0.0f, 1.0f};

    Mat4 view = mat4_from_rows(&row0, &row1, &row2, &row3);

    return view;
}

Mat4 cam_get_perspective_projection() {
    float f = 1.0f / tan(FOV / 2.0f);
    float rng_inv = 1.0f / (NEAR - FAR);
    Mat4 proj = {
        {
            f / ASPECT_RATIO, 0.0f, 0.0f, 0.0f,
            0.0f, f, 0.0f, 0.0f,
            0.0f, 0.0f, (NEAR + FAR) * rng_inv, NEAR * FAR * rng_inv * 2.0f,
            0.0f, 0.0f, -1.0f, 0.0f
        }
    };
    return proj;
}

