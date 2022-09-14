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
    Vec3 res = { { v->data[0] / len, v->data[1] / len, v->data[2] / len } };
    return res;
}

Vec3 vec3_negate(Vec3* v) {
    Vec3 res = { {-(v->data[0]), -(v->data[1]), -(v->data[2])} };
    return res;
}

Vec3 vec3_scale(Vec3* v, float s) {
    Vec3 res = { {v->data[0] * s, v->data[1] * s, v->data[2] * s} };
    return res;
}

Vec4 vec3_append(Vec3* v, float x) {
    Vec4 res = { {v->data[0], v->data[1], v->data[2], x} }; 
    return res;
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

Mat3 mat3_matmul(Mat3* m1, Mat3* m2) {
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

Mat4 mat4_matmul(Mat4* m1, Mat4* m2) {
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

    Mat3 r1 = mat3_matmul(&z_r, &y_r);
    Mat3 m = mat3_matmul(&r1, &x_r);

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

Vec3 mat4_vec3_mul(Mat4* m, Vec3* v) {
    Vec3 res;
    
    for (size_t i = 0; i < 3; ++i) {
        float val = 0.0f;
        for (size_t j = 0; j < 4; ++j) {
            if (j == 3) {
                val += m->data[i * 4 + j];
            } else {
                val += m->data[i * 4 + j] * v->data[j];
            }
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

void mat4_print(Mat4* m) {
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            printf("%f   ", m->data[i * 4 + j]);
        }
        printf("\n");
    }
}

void mat3_print(Mat3* m) {
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            printf("%f   ", m->data[i * 3 + j]);
        }
        printf("\n");
    }
}
