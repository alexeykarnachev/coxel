typedef struct Camera {
    float fov;
    float near;
    float far;
    float aspect;

    Vec3 start_pos;
    Vec3 rotation;
    Vec3 translation;
    Vec3 up;
    Vec3 view_dir;

    Mat4 view_mat;
    Mat4 proj_mat;
} Camera;

bool camera_create(Camera* cam, float fov, float near, float far, float aspect, Vec3 start_pos);
static Mat3 camera_get_basis_mat(Camera* cam);
void camera_translate(Camera* cam, float xd, float yd, float zd);
void camera_rotate(Camera* cam, float pitch, float yaw);
static void camera_update_view_mat(Camera* cam);
static void camera_update_proj_mat(Camera* cam);


bool camera_create(Camera* cam, float fov, float near, float far, float aspect, Vec3 start_pos) {
    clear_struct(cam);

    Vec3 up = {{0.0, 1.0, 0.0}};
    Vec3 view_dir = {{0.0, 0.0, -1.0}};

    cam->fov = deg2rad(fov);
    cam->near = near;
    cam->far = far;
    cam->aspect = aspect;
    cam->start_pos = start_pos;
    cam->rotation = vec3_zeros();
    cam->translation = vec3_zeros();
    cam->up = up;
    cam->view_dir = view_dir;

    camera_update_view_mat(cam);
    camera_update_proj_mat(cam);
}

static Mat3 camera_get_basis_mat(Camera* cam) {
    Mat3 rotation = mat3_rotation(cam->rotation.data[0], cam->rotation.data[1], 0.0f);
    Vec3 view_dir = mat3_vec3_mul(&rotation, &cam->view_dir);
    Mat3 basis = get_basis_mat(&view_dir, &cam->up);
    return basis;
}

void camera_translate(Camera* cam, float xd, float yd, float zd) {
    Mat3 basis = camera_get_basis_mat(cam);

    Vec3 x = mat3_get_row(&basis, 0);
    x = vec3_scale(&x, xd);

    Vec3 y = mat3_get_row(&basis, 1);
    y = vec3_scale(&y, yd);

    Vec3 z = mat3_get_row(&basis, 2);
    z = vec3_scale(&z, zd);

    cam->translation.data[0] += x.data[0] + y.data[0] + z.data[0];
    cam->translation.data[1] += x.data[1] + y.data[1] + z.data[1];
    cam->translation.data[2] += x.data[2] + y.data[2] + z.data[2];

    camera_update_view_mat(cam);
}

void camera_rotate(Camera* cam, float pitch, float yaw) {
    // TODO: mod by PI
    cam->rotation.data[0] += pitch;
    cam->rotation.data[1] += yaw;

    camera_update_view_mat(cam);
}

static void camera_update_view_mat(Camera* cam) {
    Vec3 pos = {{
        cam->start_pos.data[0] + cam->translation.data[0],
        cam->start_pos.data[1] + cam->translation.data[1],
        cam->start_pos.data[2] + cam->translation.data[2]
    }};

    Mat3 rotation = mat3_rotation(cam->rotation.data[0], cam->rotation.data[1], 0.0f);
    Vec3 view_dir = mat3_vec3_mul(&rotation, &cam->view_dir);
    cam->view_mat = get_view_mat(&view_dir, &cam->up, &pos);
}

static void camera_update_proj_mat(Camera* cam) {
    cam->proj_mat = get_perspective_projection_mat(cam->fov, cam->near, cam->far, cam->aspect);
}

