typedef struct Vec3 {
    float data[3];
} Vec3;

typedef struct Vec4 {
    float data[4];
} Vec4;

typedef struct Mat3 {
    Vec3 row0;
    Vec3 row1;
    Vec3 row2;
} Mat3;

typedef struct Mat4 {
    Vec4 row0;
    Vec4 row1;
    Vec4 row2;
    Vec4 row3;
} Mat4;

Vec3 cross(Vec3 v1, Vec3 v2) {
    Vec3 res = {
        {
            v1.data[1] * v2.data[2] - v1.data[2] * v2.data[1],
            v1.data[2] * v2.data[0] - v1.data[0] * v2.data[2],
            v1.data[0] * v2.data[1] - v1.data[1] * v2.data[0]
        }
    };
    return res;
}

float dot(Vec3 v1, Vec3 v2) {
    float res = 0.0f;
    for (size_t i = 0; i < 3; ++i) {
        res += v1.data[i] * v2.data[i];
    }
    return res;
}

float length(Vec3 v) {
    float res = 0.0f;
    for (size_t i = 0; i < 3; ++i) {
        res += v.data[i] * v.data[i];
    }
    return sqrt(res);
}

Vec3 norm(Vec3 v) {
    float len = length(v);
    Vec3 res = { { v.data[0] / len, v.data[1] / len, v.data[2] / len } };
    return res;
}

Vec3 negate(Vec3 v) {
    Vec3 res = { {-v.data[0], -v.data[1], -v.data[2]} };
    return res;
}

Vec3 scale(Vec3 v, float s) {
    Vec3 res = { {v.data[0] * s, v.data[1] * s, v.data[2] * s} };
    return res;
}

Vec4 append(Vec3 v, float x) {
    Vec4 res = { {v.data[0], v.data[1], v.data[2], x} }; 
    return res;
}
