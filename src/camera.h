static Vec3 VIEW_DIR = { {0.0f, 0.0f, -1.0f} };
static Vec3 POSITION = { {0.0f, 0.0f, 2.0f} };
static Vec3 UP = { {0.0f, 1.0f, 0.0f} };

static float FOV = 45.0f;
static float NEAR = 0.1f;
static float FAR = 100.0f;
static float ASPECT_RATIO = 4.0f / 3.0f;

static Mat3 get_basis() {
    Vec3 z = norm(VIEW_DIR);
    Vec3 x = norm(cross(z, UP));
    Vec3 y = norm(cross(x, z));
    z = negate(z);
    Mat3 basis = {x, y, z};
    return basis;
}

Mat4 get_view() {
    Mat3 basis = get_basis();
    Mat4 view = {
        append(basis.row0, -dot(basis.row0, POSITION)),
        append(basis.row1, -dot(basis.row1, POSITION)),
        append(basis.row2, -dot(basis.row2, POSITION)),
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    return view;
}

Mat4 get_perspective_projection() {
    float f = 1.0f / tan(FOV / 2.0f);
    float rng_inv = 1.0f / (NEAR - FAR);
    Mat4 proj = {
        {f / ASPECT_RATIO, 0.0f, 0.0f, 0.0f},
        {0.0f, f, 0.0f, 0.0f},
        {0.0f, 0.0f, (NEAR + FAR) * rng_inv, NEAR * FAR * rng_inv * 2.0f},
        {0.0f, 0.0f, -1.0f, 0.0f}
    };
    return proj;
}

