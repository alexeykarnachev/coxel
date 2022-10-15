typedef struct Material {
    Vec3 diffuse_color;
    Vec3 ambient_color;
    Vec3 specular_color;
    float shininess;
} Material;

void material_create(
    Material* material,
    Vec3 diffuse_color,
    Vec3 ambient_color,
    Vec3 specular_color,
    float shininess
) {
    clear_struct(material);

    material->diffuse_color = diffuse_color;
    material->ambient_color = ambient_color;
    material->specular_color = specular_color;
    material->shininess = shininess;
}

