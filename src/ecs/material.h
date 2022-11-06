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
    Vec3 constant_color,
    float shininess
) {
    Material* material = &_MATERIALS_ARENA[_N_MATERIALS++];

    material->diffuse_color = diffuse_color;
    material->ambient_color = ambient_color;
    material->specular_color = specular_color;
    material->constant_color = constant_color;
    material->shininess = shininess;

    return material;
}

Material* material_create_default() {
    Vec3 diffuse_color = {{0.7, 0.7, 0.7}};
    Vec3 ambient_color = {{0.007, 0.007, 0.007}};
    Vec3 specular_color = {{1.0, 1.0, 1.0}};
    Vec3 constant_color = {{0.0, 0.0, 0.0}};
    float shininess = 128.0;

    return material_create(diffuse_color, ambient_color, specular_color, constant_color, shininess);
}
