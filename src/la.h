#pragma once
#include <stddef.h>

typedef struct Vec2 {
    float data[2];
} Vec2;

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

typedef struct CubemapViewProj {
    float data[96];
} CubemapViewProj;

extern Vec3 vec3_zeros;
extern Vec3 vec3_ones;
extern Vec3 vec3_pos_x;
extern Vec3 vec3_neg_x;
extern Vec3 vec3_pos_y;
extern Vec3 vec3_neg_y;
extern Vec3 vec3_pos_z;
extern Vec3 vec3_neg_z;
extern Mat3 mat3_zeros;
extern Mat3 mat3_identity;
extern Mat4 mat4_identity;

Vec2 vec2(float x, float y);
Vec3 vec3(float x, float y, float z);
Vec4 vec4(float x, float y, float z, float w);

Vec3 vec3_cross(Vec3* v1, Vec3* v2);
float vec2_dot(Vec2* v1, Vec2* v2);
float vec3_dot(Vec3* v1, Vec3* v2);
float vec2_length(Vec2* v);
float vec3_length(Vec3* v);
Vec3 vec3_norm(Vec3* v);
Vec2 vec2_norm(Vec2* v);
Vec2 vec2_sum(Vec2* v1, Vec2* v2);
float vec2_angle(Vec2* v1, Vec2* v2);

Vec3 vec3_sum(Vec3* v1, Vec3* v2);
Vec2 vec2_diff(Vec2* v1, Vec2* v2);
Vec3 vec3_diff(Vec3* v1, Vec3* v2);
Vec3 vec3_negate(Vec3* v);
Vec3 vec3_scale(Vec3* v, float s);
Vec4 vec3_append(Vec3* v, float x);
Vec3 vec3_add_vals(Vec3 v, float x, float y, float z);
Vec4 vec3_to_vec4(Vec3* v, float w);
Vec2 vec3_to_vec2(Vec3* v);
Vec3 vec4_to_vec3(Vec4* v);
Mat4 mat4_from_rows(Vec4* row0, Vec4* row1, Vec4* row2, Vec4* row3);
Mat4 mat4_from_cols(Vec4* col0, Vec4* col1, Vec4* col2, Vec4* col3);
Mat4 mat3_to_mat4(Mat3* m);
Mat3 mat3_from_rows(Vec3* row0, Vec3* row1, Vec3* row2);
Mat3 mat3_from_cols(Vec3* col0, Vec3* col1, Vec3* col2);
Mat3 mat4_to_mat3(Mat4* m);
Vec4 mat4_get_row(Mat4* m, int i);
Vec3 mat3_get_row(Mat3* m, int i);
Vec4 mat4_get_col(Mat4* m, int i);
Vec3 mat3_get_col(Mat3* m, int i);
Mat3 mat3_get_x_rotation(float a);
Mat3 mat3_get_y_rotation(float a);
Mat3 mat3_get_z_rotation(float a);
Mat3 mat3_mat3_mul(Mat3* m1, Mat3* m2);
Mat4 mat4_mat4_mul(Mat4* m1, Mat4* m2);
Mat3 mat3_rotation(Vec3* rotation);
Vec3 mat3_vec3_mul(Mat3* m, Vec3* v);
Vec4 mat4_vec4_mul(Mat4* m, Vec4* v);
Vec4 mat4_vec3_mul(Mat4* m, Vec3* v);
Mat3 mat3_transpose(Mat3* m);
Mat4 mat4_transpose(Mat4* m);
Mat4 mat4_inverse(Mat4* m);
Mat4 mat4_translation(Vec3* translation);
Mat4 get_world_mat(Vec3* scale, Mat3* rotation_mat, Vec3* translation);
Mat4 get_perspective_projection_mat(
    float fov, float near, float far, float aspect
);
Mat3 get_basis_mat(Vec3* z, Vec3* up);
Mat4 get_view_mat(Vec3* view_dir, Vec3* up, Vec3* pos);
Vec3 model_mat_extract_scale_vec(Mat4* mat);
Vec3 model_mat_extract_translation_vec(Mat4* mat);
Mat3 model_mat_extract_rotation_mat(Mat4* mat);
Mat4 model_mat_scale(Mat4 mat, Vec3* scale);
void mat4_print(Mat4* m);
void mat3_print(Mat3* m);
void vec2_print(Vec2* v);
void vec3_print(Vec3* v);
void vec4_print(Vec4* v);
void mat4_pack(float dst[], Mat4 mats[], size_t n_mats);
void mat4_transpose_pack(float dst[], Mat4 mats[], size_t n_mats);
void vec3_pack(float dst[], Vec3 vecs[], size_t n_vecs);
CubemapViewProj get_cubemap_view_proj(
    float near_plane, float far_plane, Vec3* world_pos
);
int isect_line_plane(
    Vec3* out_p,
    Vec3* line_p0,
    Vec3* line_p1,
    Vec3* plane_p,
    Vec3* plane_normal
);
int get_two_vecs_nearest_point(
    Vec3* vec0_out_nearest_point,
    Vec3* vec0_p0,
    Vec3* vec0_p1,
    Vec3* vec1_p0,
    Vec3* vec1_p1
);
Vec3 vec3_project(Vec3* position, Mat4* proj_mat);

void mat4_print(Mat4* m);
void mat3_print(Mat3* m);
void vec3_print(Vec3* v);
void vec4_print(Vec4* v);

int is_point_inside_rect(
    float rect_x,
    float rect_y,
    float rect_w,
    float rect_h,
    float point_x,
    float point_y
);
