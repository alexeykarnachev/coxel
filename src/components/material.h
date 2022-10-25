#define _MATERIAL_UBO_N_BYTES 64

typedef struct Material {
    Vec3 diffuse_color;
    Vec3 ambient_color;
    Vec3 specular_color;
    float shininess;
} Material;

Material _MATERIAL_ARENA[MAX_N_MATERIALS];
size_t N_MATERIALS = 0;
int _MATERIAL_UBO = -1;


Material* material_get(size_t id) {
    return &_MATERIAL_ARENA[id - MATERIAL_START_ID]
}

void _material_pack(Material* material, float dst[]) {
    size_t size = sizeof(float);
    memcpy(&dst[0], material->diffuse_color.data, size * 3);
    memcpy(&dst[4], material->ambient_color.data, size * 3);
    memcpy(&dst[8], material->specular_color.data, size * 3);
    dst[12] = material->shininess;
}

void _material_create_ubo() {
    glGenBuffers(1, &_MATERIAL_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, _MATERIAL_UBO);
    glBufferData(
        GL_UNIFORM_BUFFER,
        _MATERIAL_UBO_N_BYTES * MAX_N_MATERIALS,
        NULL,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, MATERIAL_BINDING_IDX, _MATERIAL_UBO);
}

void _material_update_ubo(size_t material_id) {
    if (_MATERIAL_UBO == -1) {
        _material_create_ubo();
    }

    static float data[_MATERIAL_UBO_N_BYTES / 4];
    _material_pack(get_materials(material_id), data);

    glBindBuffer(GL_UNIFORM_BUFFER, _MATERIAL_UBO);
    glBufferSubData(
        GL_UNIFORM_BUFFER,
        material_id * _MATERIAL_UBO_N_BYTES,
        _MATERIAL_UBO_N_BYTES,
        data
    );

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

int material_create(
    Vec3 diffuse_color,
    Vec3 ambient_color,
    Vec3 specular_color,
    float shininess
) {
    if (N_MATERIALS == MAX_N_MATERIALS) {
        fprintf(stderr, "ERROR: max number of materials is reached. Material won't be created");
        return -1;
    }
    size_t id = MATERIAL_START_ID + (N_MATERIALS++);
    Material* material = material_get(id);

    material->diffuse_color = diffuse_color;
    material->ambient_color = ambient_color;
    material->specular_color = specular_color;
    material->shininess = shininess;

    _material_update_ubo(id);
    return id;
}

