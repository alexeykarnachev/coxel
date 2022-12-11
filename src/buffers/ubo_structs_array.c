#include "ubo_structs_array.h"

bool ubo_structs_array_create(
    UBOStructsArray* ubo,
    size_t max_n_structs,
    size_t struct_size,
    size_t binding_idx,
    GLenum usage
) {
    glGenBuffers(1, &ubo->ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo->ubo);
    glBufferData(
        GL_UNIFORM_BUFFER, struct_size * max_n_structs + 16, NULL, usage
    );

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_idx, ubo->ubo);

    ubo->max_n_structs = max_n_structs;
    ubo->struct_size = struct_size;
    ubo->n_structs = 0;

    return true;
}

bool ubo_structs_array_add(UBOStructsArray* ubo, float packed_struct[]) {
    if (ubo->max_n_structs == ubo->n_structs) {
        fprintf(
            stderr,
            "ERROR: can't add another struct to the ubo structs array. "
            "Max number of structs is reached\n"
        );
        return false;
    }

    size_t struct_idx = ubo->n_structs++;
    glBindBuffer(GL_UNIFORM_BUFFER, ubo->ubo);
    glBufferSubData(
        GL_UNIFORM_BUFFER,
        struct_idx * ubo->struct_size,
        ubo->struct_size,
        packed_struct
    );
    glBufferSubData(
        GL_UNIFORM_BUFFER,
        ubo->max_n_structs * ubo->struct_size,
        16,
        &ubo->n_structs
    );
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return true;
}

bool ubo_structs_array_update(
    UBOStructsArray* ubo, float packed_struct[], size_t struct_idx
) {
    if (struct_idx == ubo->n_structs) {
        fprintf(
            stderr,
            "ERROR: can't add another struct to the ubo structs array. "
            "Max number of structs is reached\n"
        );
        return false;
    }
    ubo->n_structs += 1;

    glBindBuffer(GL_UNIFORM_BUFFER, ubo->ubo);
    glBufferSubData(
        GL_UNIFORM_BUFFER,
        struct_idx * ubo->struct_size,
        ubo->struct_size,
        packed_struct
    );
    glBufferSubData(
        GL_UNIFORM_BUFFER,
        ubo->max_n_structs * ubo->struct_size,
        16,
        &ubo->n_structs
    );
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return true;
}
