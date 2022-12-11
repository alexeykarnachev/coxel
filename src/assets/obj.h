#pragma once
#include <stddef.h>
#include <stdint.h>

// TODO: Factor out these arguments into a return structure
int load_obj(
    char* file_path,
    float** vp,
    size_t* vp_size,
    float** vn,
    size_t* vn_size,
    uint32_t** f,
    size_t* f_size
);
