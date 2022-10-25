#define MATERIAL_PACK_SIZE 64

typedef struct Material {
    Vec3 diffuse_color;
    Vec3 ambient_color;
    Vec3 specular_color;
    float shininess;
} Material;


Material material_create(
    Vec3 diffuse_color,
    Vec3 ambient_color,
    Vec3 specular_color,
    float shininess
) {
    Material material;

    material.diffuse_color = diffuse_color;
    material.ambient_color = ambient_color;
    material.specular_color = specular_color;
    material.shininess = shininess;

    return material;
}

void material_pack(Material* material, float dst[]) {
    size_t size = sizeof(float);
    memcpy(&dst[0], material->diffuse_color.data, size * 3);
    memcpy(&dst[4], material->ambient_color.data, size * 3);
    memcpy(&dst[8], material->specular_color.data, size * 3);
    dst[12] = material->shininess;
}

