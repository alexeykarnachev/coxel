#include "material.h"

#include "../defaults.h"

static Material MATERIALS_ARENA[1 << 14];
static size_t N_MATERIALS = 0;

Material* material_create(Vec3 diffuse_color, float specular) {
    Material* material = &MATERIALS_ARENA[N_MATERIALS++];

    material->diffuse_color = diffuse_color;
    material->specular = specular;

    return material;
}

Material* material_create_default() {
    return material_create(
        DEFAULT_MATERIAL_DIFFUSE_COLOR, DEFAULT_MATERIAL_SPECULAR
    );
}
