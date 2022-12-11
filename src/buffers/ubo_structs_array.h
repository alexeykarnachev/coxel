#pragma once
#include <glad/glad.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct UBOStructsArray {
    GLuint ubo;
    size_t max_n_structs;
    size_t struct_size;
    size_t n_structs;
} UBOStructsArray;

bool ubo_structs_array_create(
    UBOStructsArray* ubo,
    size_t max_n_structs,
    size_t struct_size,
    size_t binding_idx,
    GLenum usage
);
bool ubo_structs_array_add(UBOStructsArray* ubo, float packed_struct[]);
bool ubo_structs_array_update(
    UBOStructsArray* ubo, float packed_struct[], size_t struct_idx
);
