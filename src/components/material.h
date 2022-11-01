#define MATERIAL_PACK_SIZE 64

typedef struct Material {
    Vec3 diffuse_color;
    Vec3 ambient_color;
    Vec3 specular_color;
    float shininess;
} Material;


void material_pack(Material* material, float dst[]) {
    memcpy(&dst[0], material->diffuse_color.data, 12);
    memcpy(&dst[4], material->ambient_color.data, 12);
    memcpy(&dst[8], material->specular_color.data, 12);
    dst[12] = material->shininess;
}

