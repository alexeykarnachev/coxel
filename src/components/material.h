typedef struct Material {
    Vec3 diffuse_color;
    Vec3 specular_color;
} Material;

Material _MATERIALS_ARENA[1 << 14];
size_t _N_MATERIALS = 0;


Material* material_create(Vec3 diffuse_color, Vec3 specular_color) {
    Material* material = &_MATERIALS_ARENA[_N_MATERIALS++];

    material->diffuse_color = diffuse_color;
    material->specular_color = specular_color;

    return material;
}

Material* material_create_default() {
    return material_create(
        DEFAULT_MATERIAL_DIFFUSE_COLOR,
        DEFAULT_MATERIAL_SPECULAR_COLOR
    );
}
