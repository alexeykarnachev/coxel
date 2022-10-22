#define _MATERIAL_MAX_N 1024
#define _MATERIAL_UBO_N_BYTES 52
#define _MATERIAL_UBO_N_ELEMENTS 13

typedef struct Material {
    Vec3 diffuse_color;
    Vec3 ambient_color;
    Vec3 specular_color;
    float shininess;

    GLuint ubo;
} Material;

Material _MATERIAL_ARENA[_MATERIAL_MAX_N];
size_t _MATERIAL_ARENA_IDX = 0;


void material_pack(Material* material, float dst[]) {
    size_t size = sizeof(float);
    memcpy(&dst[0], material->diffuse_color.data, size * 3);
    memcpy(&dst[4], material->ambient_color.data, size * 3);
    memcpy(&dst[8], material->specular_color.data, size * 3);
    dst[12] = material->shininess;
}

Material* material_create(
    Vec3 diffuse_color,
    Vec3 ambient_color,
    Vec3 specular_color,
    float shininess
) {
    if (_MATERIAL_ARENA_IDX == _MATERIAL_MAX_N) {
        fprintf(stderr, "ERROR: max number of materials is reached. Material won't be created");
        return NULL;
    }
    Material* material = &_MATERIAL_ARENA[_MATERIAL_ARENA_IDX];
    _MATERIAL_ARENA_IDX += 1;

    material->diffuse_color = diffuse_color;
    material->ambient_color = ambient_color;
    material->specular_color = specular_color;
    material->shininess = shininess;

    static float data[_MATERIAL_UBO_N_ELEMENTS];
    material_pack(material, data);

    glGenBuffers(1, &material->ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, material->ubo);
    glBufferData(GL_UNIFORM_BUFFER, _MATERIAL_UBO_N_BYTES, data, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return material;
}

void material_bind(Material* material) {
    glBindBufferBase(GL_UNIFORM_BUFFER, MATERIAL_BINDING_IDX, material->ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, material->ubo);
}
