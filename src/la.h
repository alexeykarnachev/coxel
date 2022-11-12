typedef struct Vec3 {
    float data[3];
} Vec3;

typedef struct Vec4 {
    float data[4];
} Vec4;

typedef struct Mat3 {
    float data[9];
} Mat3;

typedef struct Mat4 {
    float data[16];
} Mat4;

Vec3 vec3(float x, float y, float z) {
    Vec3 v = {{x, y, z}};
    return v;
}

Vec4 vec4(float x, float y, float z, float w) {
    Vec4 v = {{x, y, z, w}};
    return v;
}

void mat4_print(Mat4* m);
void mat3_print(Mat3* m);
void vec3_print(Vec3* v);
void vec4_print(Vec4* v);

Vec3 vec3_pos_x = {{1.0, 0.0, 0.0}};
Vec3 vec3_neg_x = {{-1.0, 0.0, 0.0}};
Vec3 vec3_pos_y = {{0.0, 1.0, 0.0}};
Vec3 vec3_neg_y = {{0.0, -1.0, 0.0}};
Vec3 vec3_pos_z = {{0.0, 0.0, 1.0}};
Vec3 vec3_neg_z = {{0.0, 0.0, -1.0}};

Vec3 vec3_cross(Vec3* v1, Vec3* v2) {
    Vec3 res = {
        {
            v1->data[1] * v2->data[2] - v1->data[2] * v2->data[1],
            v1->data[2] * v2->data[0] - v1->data[0] * v2->data[2],
            v1->data[0] * v2->data[1] - v1->data[1] * v2->data[0]
        }
    };
    return res;
}

float vec3_dot(Vec3* v1, Vec3* v2) {
    float res = 0.0f;
    for (size_t i = 0; i < 3; ++i) {
        res += v1->data[i] * v2->data[i];
    }
    return res;
}

float vec3_length(Vec3* v) {
    float res = 0.0f;
    for (size_t i = 0; i < 3; ++i) {
        res += v->data[i] * v->data[i];
    }
    return sqrt(res);
}

Vec3 vec3_norm(Vec3* v) {
    float len = vec3_length(v);
    Vec3 res = {{ v->data[0] / len, v->data[1] / len, v->data[2] / len }};
    return res;
}

Vec3 vec3_sum(Vec3* v1, Vec3* v2) {
    Vec3 res = {{v1->data[0] + v2->data[0], v1->data[1] + v2->data[1], v1->data[2] + v2->data[2]}};
    return res;
}

Vec3 vec3_diff(Vec3* v1, Vec3* v2) {
    Vec3 res = {{v1->data[0] - v2->data[0], v1->data[1] - v2->data[1], v1->data[2] - v2->data[2]}};
    return res;
}

Vec3 vec3_negate(Vec3* v) {
    Vec3 res = {{-(v->data[0]), -(v->data[1]), -(v->data[2])}};
    return res;
}

Vec3 vec3_scale(Vec3* v, float s) {
    Vec3 res = {{v->data[0] * s, v->data[1] * s, v->data[2] * s}};
    return res;
}

Vec4 vec3_append(Vec3* v, float x) {
    Vec4 res = {{v->data[0], v->data[1], v->data[2], x}}; 
    return res;
}

Vec3 vec3_add_vals(Vec3 v, float x, float y, float z) {
    v.data[0] += x;
    v.data[1] += y;
    v.data[2] += z;
}

Mat4 mat4_from_rows(Vec4* row0, Vec4* row1, Vec4* row2, Vec4* row3) {
    Mat4 mat = {
        {
            row0->data[0], row0->data[1], row0->data[2], row0->data[3],
            row1->data[0], row1->data[1], row1->data[2], row1->data[3],
            row2->data[0], row2->data[1], row2->data[2], row2->data[3],
            row3->data[0], row3->data[1], row3->data[2], row3->data[3]
        }
    };

    return mat;
}

Mat4 mat4_from_cols(Vec4* col0, Vec4* col1, Vec4* col2, Vec4* col3) {
    Mat4 mat = {
        {
            col0->data[0], col1->data[0], col2->data[0], col3->data[0],
            col0->data[1], col1->data[1], col2->data[1], col3->data[1],
            col0->data[2], col1->data[2], col2->data[2], col3->data[2],
            col0->data[3], col1->data[3], col2->data[3], col3->data[3]
        }
    };

    return mat;
}

Mat3 mat3_from_rows(Vec3* row0, Vec3* row1, Vec3* row2) {
    Mat3 mat = {
        {
            row0->data[0], row0->data[1], row0->data[2],
            row1->data[0], row1->data[1], row1->data[2],
            row2->data[0], row2->data[1], row2->data[2]
        }
    };

    return mat;
}

Mat3 mat3_from_cols(Vec3* col0, Vec3* col1, Vec3* col2) {
    Mat3 mat = {
        {
            col0->data[0], col1->data[0], col2->data[0],
            col0->data[1], col1->data[1], col2->data[1],
            col0->data[2], col1->data[2], col2->data[2]
        }
    };

    return mat;
}

Vec4 mat4_get_row(Mat4* m, int i) {
    Vec4 v;
    for (size_t j = 0; j < 4; ++j) {
        v.data[j] = m->data[i * 4 + j];
    }

    return v;
}

Vec3 mat3_get_row(Mat3* m, int i) {
    Vec3 v;
    for (size_t j = 0; j < 3; ++j) {
        v.data[j] = m->data[i * 3 + j];
    }

    return v;
}

Vec4 mat4_get_col(Mat4* m, int i) {
    Vec4 v;
    for (size_t j = 0; j < 4; ++j) {
        v.data[j] = m->data[i + 4 * j];
    }

    return v;
}

Vec3 mat3_get_col(Mat3* m, int i) {
    Vec3 v;
    for (size_t j = 0; j < 3; ++j) {
        v.data[j] = m->data[i + 3 * j];
    }

    return v;
}

Mat3 mat3_get_x_rotation(float a) {
    float c = cos(a);
    float s = sin(a);
    Mat3 m = {
        {
            1.0f,  0.0f,  0.0f,
            0.0f,  c,     -s,
            0.0f,  s,     c
        }
    };

    return m;
}

Mat3 mat3_get_y_rotation(float a) {
    float c = cos(a);
    float s = sin(a);
    Mat3 m = {
        {
            c,     0.0f,  s,
            0.0f,  1.0f,  0.0f,
            -s,    0.0f,  c
        }
    };

    return m;
}

Mat3 mat3_get_z_rotation(float a) {
    float c = cos(a);
    float s = sin(a);
    Mat3 m = {
        {
            c,     -s,    0.0f,
            s,     c,     0.0f,
            0.0f,  0.0f,  1.0f
        }
    };

    return m;
}

Mat3 mat3_mat3_mul(Mat3* m1, Mat3* m2) {
    Mat3 m;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            float val = 0.0f;
            for (size_t k = 0; k < 3; ++k) {
                val += m1->data[3 * i + k] * m2->data[j + 3 * k];
            }
            m.data[i * 3 + j] = val;
        }
    }

    return m;
}

Mat4 mat4_mat4_mul(Mat4* m1, Mat4* m2) {
    Mat4 m;
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            float val = 0.0f;
            for (size_t k = 0; k < 4; ++k) {
                val += m1->data[4 * i + k] * m2->data[j + 4 * k];
            }
            m.data[i * 4 + j] = val;
        }
    }

    return m;
}

Mat3 mat3_rotation(float xa, float ya, float za) {
    Mat3 x_r = mat3_get_x_rotation(xa);
    Mat3 y_r = mat3_get_y_rotation(ya);
    Mat3 z_r = mat3_get_z_rotation(za);

    Mat3 r1 = mat3_mat3_mul(&y_r, &x_r);
    Mat3 m = mat3_mat3_mul(&z_r, &r1);

    return m;
}

Vec3 mat3_vec3_mul(Mat3* m, Vec3* v) {
    Vec3 res;
    
    for (size_t i = 0; i < 3; ++i) {
        float val = 0.0f;
        for (size_t j = 0; j < 3; ++j) {
            val += m->data[i * 3 + j] * v->data[j];
        }
        res.data[i] = val;
    }

    return res;
}

Vec4 mat4_vec4_mul(Mat4* m, Vec4* v) {
    Vec4 res;
    
    for (size_t i = 0; i < 4; ++i) {
        float val = 0.0f;
        for (size_t j = 0; j < 4; ++j) {
            val += m->data[i * 4 + j] * v->data[j];
        }
        res.data[i] = val;
    }

    return res;
}

Vec4 mat4_vec3_mul(Mat4* m, Vec3* v) {
    Vec4 h = vec3_append(v, 1.0f);
    return mat4_vec4_mul(m, &h);
}

Mat4 mat4_transpose(Mat4* m) {
    Mat4 t;
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            t.data[i * 4 + j] = m->data[j * 4 + i];
        }
    }
    return t;
}

Mat4 get_world_mat(Vec3* scale, Vec3* rotation, Vec3* translation) {
    Mat3 scale_mat = {{
        scale->data[0], 0.0, 0.0,
        0.0, scale->data[1], 0.0,
        0.0, 0.0, scale->data[2]
    }};

    Mat3 rotation_mat = mat3_rotation(
        rotation->data[0],
        rotation->data[1],
        rotation->data[2]
    );
    Mat3 rs_mat = mat3_mat3_mul(&rotation_mat, &scale_mat);
    Mat4 world_mat = {{
        rs_mat.data[0], rs_mat.data[1], rs_mat.data[2], translation->data[0],
        rs_mat.data[3], rs_mat.data[4], rs_mat.data[5], translation->data[1],
        rs_mat.data[6], rs_mat.data[7], rs_mat.data[8], translation->data[2],
        0.0f, 0.0f, 0.0f, 1.0f
    }};

    return world_mat;
}

Mat4 get_perspective_projection_mat(float fov, float near, float far, float aspect) {
    float f = 1.0f / tan(fov / 2.0f);
    float rng_inv = 1.0f / (near - far);
    
    Mat4 proj = {
        {
            f / aspect, 0.0f, 0.0f, 0.0f,
            0.0f, f, 0.0f, 0.0f,
            0.0f, 0.0f, (near + far) * rng_inv, near * far * rng_inv * 2.0f,
            0.0f, 0.0f, -1.0f, 0.0f
        }
    };

    return proj;
}

Mat3 get_basis_mat(Vec3* z, Vec3* up) {
    Vec3 z_norm = vec3_norm(z);

    Vec3 x = vec3_cross(&z_norm, up);
    x = vec3_norm(&x);

    Vec3 y = vec3_cross(&x, &z_norm);
    y = vec3_norm(&y);

    Mat3 basis = mat3_from_rows(&x, &y, &z_norm);
    return basis;
}

Mat4 get_view_mat(Vec3* view_dir, Vec3* up, Vec3* pos) {
    Mat3 basis = get_basis_mat(view_dir, up);
    Vec3 x = mat3_get_row(&basis, 0);
    Vec3 y = mat3_get_row(&basis, 1);
    Vec3 z = mat3_get_row(&basis, 2);
    z = vec3_negate(&z);

    Vec4 row0 = vec3_append(&x, -vec3_dot(&x, pos)); 
    Vec4 row1 = vec3_append(&y, -vec3_dot(&y, pos));
    Vec4 row2 = vec3_append(&z, -vec3_dot(&z, pos));
    Vec4 row3 = {0.0f, 0.0f, 0.0f, 1.0f};

    Mat4 view = mat4_from_rows(&row0, &row1, &row2, &row3);
    return view;
}

void mat4_print(Mat4* m) {
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            printf("%f   ", m->data[i * 4 + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void mat3_print(Mat3* m) {
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            printf("%f   ", m->data[i * 3 + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void vec3_print(Vec3* v) {
    for (size_t i = 0; i < 3; ++i) {
        printf("%f   ", v->data[i]);
    }
    printf("\n");
}

void vec4_print(Vec4* v) {
    for (size_t i = 0; i < 4; ++i) {
        printf("%f   ", v->data[i]);
    }
    printf("\n");
}

Vec3 vec3_zeros() {
    Vec3 vec = {{0.0f, 0.0f, 0.0f}};
    return vec;
}

Vec3 vec3_ones() {
    Vec3 vec = {{1.0f, 1.0f, 1.0f}};
    return vec;
}

Mat4 mat4_identity() {
    Mat4 mat = {{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    }};
    return mat;
}

void mat4_pack(float dst[], Mat4 mats[], size_t n_mats) {
    for (size_t i = 0; i < n_mats; ++i) {
        memcpy(&dst[i * 16], mats[i].data, sizeof(mats[i].data[0]) * 16);
    } 
}

void mat4_transpose_pack(float dst[], Mat4 mats[], size_t n_mats) {
    size_t i_loc = 0;
    for (size_t i_mat = 0; i_mat < n_mats; ++i_mat) {
        Mat4* mat = &mats[i_mat];
        for (size_t i_col = 0; i_col < 4; ++i_col) {
            for (size_t i_row = 0; i_row < 4; ++i_row) {
                dst[i_loc++] = mat->data[i_row * 4 + i_col];
            }
        }
    } 
}

void vec3_pack(float dst[], Vec3 vecs[], size_t n_vecs) {
    for (size_t i = 0; i < n_vecs; ++i) {
        memcpy(&dst[i * 3], vecs[i].data, sizeof(vecs[i].data[0]) * 3);
    } 
}

typedef struct CubemapViewProj {
    float data[96];
} CubemapViewProj;

CubemapViewProj get_cubemap_view_proj(
    float near_plane, float far_plane, Vec3* world_pos
) {
    Mat4 proj_mat = get_perspective_projection_mat(
        deg2rad(90.0), near_plane, far_plane, 1.0);

    static Mat4 view_mats[6];
    view_mats[0] = get_view_mat(&vec3_pos_x, &vec3_neg_y, world_pos);
    view_mats[1] = get_view_mat(&vec3_neg_x, &vec3_neg_y, world_pos);
    view_mats[2] = get_view_mat(&vec3_pos_y, &vec3_pos_z, world_pos);
    view_mats[3] = get_view_mat(&vec3_neg_y, &vec3_neg_z, world_pos);
    view_mats[4] = get_view_mat(&vec3_pos_z, &vec3_neg_y, world_pos);
    view_mats[5] = get_view_mat(&vec3_neg_z, &vec3_neg_y, world_pos);

    static Mat4 view_proj_mats[6];
    for (size_t i = 0; i < 6; ++i) {
        view_proj_mats[i] = mat4_mat4_mul(&proj_mat, &view_mats[i]);
    }

    CubemapViewProj cubemap_view_proj;
    mat4_transpose_pack(cubemap_view_proj.data, view_proj_mats, 6);

    return cubemap_view_proj;
}

