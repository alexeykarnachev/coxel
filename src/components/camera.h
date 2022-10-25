#define _CAMERA_UBO_N_BYTES 144

typedef struct Camera {
    float fov;
    float near;
    float far;
    float aspect;

    Vec3 pos;
    Vec3 up;
    Vec3 view_dir;
    Vec3 rotation;
    Vec3 translation;
} Camera;

Camera _CAMERA_ARENA[MAX_N_CAMERAS];
size_t N_CAMERAS = 0;
int _CAMERA_UBO = -1;


Camera* camera_get(size_t id) {
    return &_CAMERA_ARENA[id - CAMERA_START_ID];
}

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
        _CAMERA_UBO_N_BYTES * MAX_N_CAMERAS,
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
    _camera_pack(camera_get(camera_id), data);

    glBindBuffer(GL_UNIFORM_BUFFER, _CAMERA_UBO);
    glBufferSubData(
        GL_UNIFORM_BUFFER,
        camera_id * _CAMERA_UBO_N_BYTES,
        _CAMERA_UBO_N_BYTES,
        data
    );

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

int camera_create(float fov, float near, float far, float aspect, Vec3 pos) {
    if (N_CAMERAS == MAX_N_CAMERAS) {
        fprintf(stderr, "ERROR: max number of cameras is reached. Camera won't be created");
        return -1;
    }

    size_t id = CAMERA_START_ID + (N_CAMERAS++);
    Camera* cam = camera_get(id);

    Vec3 up = {{0.0, 1.0, 0.0}};
    Vec3 view_dir = {{0.0, 0.0, -1.0}};

    cam->fov = deg2rad(fov);
    cam->near = near;
    cam->far = far;
    cam->aspect = aspect;
    cam->pos = pos;
    cam->rotation = vec3_zeros();
    cam->up = up;
    cam->view_dir = view_dir;

    _camera_update_ubo(id);
    return id;
}

