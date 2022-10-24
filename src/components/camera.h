#define _CAMERA_UBO_N_BYTES 144

typedef struct Camera {
    float fov;
    float near;
    float far;
    float aspect;
    float side_sens;
    float straight_sens;
    float rotation_sens;

    Vec3 pos;
    Vec3 up;
    Vec3 view_dir;
    Vec3 rotation;
    Vec3 translation;
} Camera;

Camera CAMERA_ARENA[MAX_N_CAMERAS];
size_t _CAMERA_ARENA_IDX = 0;
int _CAMERA_UBO = -1;


void _camera_pack(Camera* cam, float dst[]) {
    Vec3 pos = vec3_sum(&cam->translation, &cam->pos);
    Mat3 rotation = mat3_rotation(cam->rotation.data[0], cam->rotation.data[1], 0.0f);
    Vec3 view_dir = mat3_vec3_mul(&rotation, &cam->view_dir);
    Mat4 view_mat = get_view_mat(&view_dir, &cam->up, &pos);
    Mat4 proj_mat = get_perspective_projection_mat(cam->fov, cam->near, cam->far, cam->aspect);

    vec3_pack(&dst[0], &pos, 1);
    mat4_transpose_pack(&dst[4], &view_mat, 1);
    mat4_transpose_pack(&dst[20], &proj_mat, 1);
}

void _camera_create_ubo() {
    glGenBuffers(1, &_CAMERA_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, _CAMERA_UBO);
    glBufferData(
        GL_UNIFORM_BUFFER,
        _CAMERA_UBO_N_BYTES * MAX_N_CAMERAS + 16,
        NULL,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BINDING_IDX, _CAMERA_UBO);
}

void _camera_update_ubo(size_t camera_id) {
    if (_CAMERA_UBO == -1) {
        _camera_create_ubo();
    }

    static float data[_CAMERA_UBO_N_BYTES / 4];
    _camera_pack(&CAMERA_ARENA[camera_id], data);

    glBindBuffer(GL_UNIFORM_BUFFER, _CAMERA_UBO);
    glBufferSubData(
        GL_UNIFORM_BUFFER,
        camera_id * _CAMERA_UBO_N_BYTES,
        _CAMERA_UBO_N_BYTES,
        data
    );

    glBufferSubData(
        GL_UNIFORM_BUFFER,
        MAX_N_CAMERAS * _CAMERA_UBO_N_BYTES,
        16,
        &_CAMERA_ARENA_IDX
    );

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

int camera_create(
    float fov,
    float near,
    float far,
    float aspect,
    float side_sens,
    float straight_sens,
    float rotation_sens,
    Vec3 pos
) {
    if (_CAMERA_ARENA_IDX == MAX_N_CAMERAS) {
        fprintf(stderr, "ERROR: max number of cameras is reached. Camera won't be created");
        return -1;
    }
    Camera* cam = &CAMERA_ARENA[_CAMERA_ARENA_IDX];

    Vec3 up = {{0.0, 1.0, 0.0}};
    Vec3 view_dir = {{0.0, 0.0, -1.0}};

    cam->fov = deg2rad(fov);
    cam->near = near;
    cam->far = far;
    cam->aspect = aspect;
    cam->side_sens = side_sens;
    cam->straight_sens = straight_sens;
    cam->rotation_sens = rotation_sens;
    cam->pos = pos;
    cam->rotation = vec3_zeros();
    cam->up = up;
    cam->view_dir = view_dir;

    _camera_update_ubo(_CAMERA_ARENA_IDX);
    return _CAMERA_ARENA_IDX++;
}

Mat3 _camera_get_basis_mat(Camera* cam) {
    Mat3 rotation = mat3_rotation(cam->rotation.data[0], cam->rotation.data[1], 0.0f);
    Vec3 view_dir = mat3_vec3_mul(&rotation, &cam->view_dir);
    Mat3 basis = get_basis_mat(&view_dir, &cam->up);
    return basis;
}

void camera_translate(Camera* cam, float dx, float dy, float dz) {
    Mat3 basis = _camera_get_basis_mat(cam);

    Vec3 x = mat3_get_row(&basis, 0);
    x = vec3_scale(&x, dx * cam->side_sens);

    Vec3 y = mat3_get_row(&basis, 1);
    y = vec3_scale(&y, dy * cam->side_sens);

    Vec3 z = mat3_get_row(&basis, 2);
    z = vec3_scale(&z, dz * cam->straight_sens);

    cam->translation.data[0] += x.data[0] + y.data[0] + z.data[0];
    cam->translation.data[1] += x.data[1] + y.data[1] + z.data[1];
    cam->translation.data[2] += x.data[2] + y.data[2] + z.data[2];
}

void camera_rotate(Camera* cam, float pitch, float yaw) {
    // TODO: mod by PI
    cam->rotation.data[0] += pitch * cam->rotation_sens;
    cam->rotation.data[1] += yaw * cam->rotation_sens;
}

void camera_set_aspect(Camera* cam, float aspect) {
    cam->aspect = aspect;
}

