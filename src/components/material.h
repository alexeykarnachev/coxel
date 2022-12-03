typedef struct Material {
    Vec3 diffuse_color;
    float specular;
} Material;

Material _MATERIALS_ARENA[1 << 14];
size_t _N_MATERIALS = 0;


Material* material_create(Vec3 diffuse_color, float specular) {
    Material* material = &_MATERIALS_ARENA[_N_MATERIALS++];

    material->diffuse_color = diffuse_color;
    material->specular = specular;

    return material;
}

Material* material_create_default() {
    return material_create(
        DEFAULT_MATERIAL_DIFFUSE_COLOR,
        DEFAULT_MATERIAL_SPECULAR
    );
}
