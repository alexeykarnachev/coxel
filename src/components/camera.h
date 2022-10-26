#define CAMERA_PACK_SIZE 144

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


Camera camera_create(float fov_deg, float near, float far, float aspect, Vec3 pos) {
    Camera cam;

    Vec3 up = {{0.0, 1.0, 0.0}};
    Vec3 view_dir = {{0.0, 0.0, -1.0}};

    cam.fov = deg2rad(fov_deg);
    cam.near = near;
    cam.far = far;
    cam.aspect = aspect;

    cam.pos = pos;
    cam.up = up;
    cam.view_dir = view_dir;
    cam.rotation = vec3_zeros();
    cam.translation = vec3_zeros();

    return cam;
}

void camera_pack(Camera* cam, float dst[]) {
    Vec3 pos = vec3_sum(&cam->translation, &cam->pos);
    Mat3 rotation = mat3_rotation(cam->rotation.data[0], cam->rotation.data[1], 0.0f);
    Vec3 view_dir = mat3_vec3_mul(&rotation, &cam->view_dir);
    Mat4 view_mat = get_view_mat(&view_dir, &cam->up, &pos);
    Mat4 proj_mat = get_perspective_projection_mat(cam->fov, cam->near, cam->far, cam->aspect);

    vec3_pack(&dst[0], &pos, 1);
    mat4_transpose_pack(&dst[4], &view_mat, 1);
    mat4_transpose_pack(&dst[20], &proj_mat, 1);
}

