typedef struct Material {
    Vec3 diffuse_color;
    Vec3 ambient_color;
    Vec3 specular_color;
    Vec3 constant_color;
    float shininess;
} Material;

Material _MATERIALS_ARENA[1 << 14];
size_t _N_MATERIALS = 0;


Material* material_create(
    Vec3 diffuse_color,
    Vec3 ambient_color,
    Vec3 specular_color,
    float shininess
) {
    Material* material = &_MATERIALS_ARENA[_N_MATERIALS++];
    Vec3 constant_color = {{0.0, 0.0, 0.0}};

    material->diffuse_color = diffuse_color;
    material->ambient_color = ambient_color;
    material->specular_color = specular_color;
    material->constant_color = constant_color;
    material->shininess = shininess;

    return material;
}

Material* material_create_default() {
    return material_create(
        DEFAULT_MATERIAL_DIFFUSE_COLOR,
        DEFAULT_MATERIAL_AMBIENT_COLOR,
        DEFAULT_MATERIAL_SPECULAR_COLOR,
        DEFAULT_MATERIAL_SHININESS
    );
}
