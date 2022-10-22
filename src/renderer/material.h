#define _MATERIAL_UBO_N_BYTES 64
#define _MATERIAL_UBO_N_ELEMENTS 16

typedef struct Material {
    Vec3 diffuse_color;
    Vec3 ambient_color;
    Vec3 specular_color;
    float shininess;
    size_t id;
} Material;

int MATERIAL_UBO = -1;
Material _MATERIAL_ARENA[MAX_N_MATERIALS];
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
    if (_MATERIAL_ARENA_IDX == MAX_N_MATERIALS) {
        fprintf(stderr, "ERROR: max number of materials is reached. Material won't be created");
        return NULL;
    }
    Material* material = &_MATERIAL_ARENA[_MATERIAL_ARENA_IDX];

    material->diffuse_color = diffuse_color;
    material->ambient_color = ambient_color;
    material->specular_color = specular_color;
    material->shininess = shininess;
    material->id = _MATERIAL_ARENA_IDX;

    static float data[_MATERIAL_UBO_N_ELEMENTS];
    material_pack(material, data);

    if (MATERIAL_UBO == -1) {
        glGenBuffers(1, &MATERIAL_UBO);
        glBindBuffer(GL_UNIFORM_BUFFER, MATERIAL_UBO);
        glBufferData(
            GL_UNIFORM_BUFFER,
            _MATERIAL_UBO_N_BYTES * MAX_N_MATERIALS + 16,
            NULL,
            GL_DYNAMIC_DRAW
        );
    }

    glBufferSubData(
        GL_UNIFORM_BUFFER,
        _MATERIAL_ARENA_IDX * _MATERIAL_UBO_N_BYTES,
        _MATERIAL_UBO_N_BYTES,
        data
    );
    _MATERIAL_ARENA_IDX += 1;

    glBufferSubData(
        GL_UNIFORM_BUFFER,
        MAX_N_MATERIALS * _MATERIAL_UBO_N_BYTES,
        16,
        &_MATERIAL_ARENA_IDX
    );

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, MATERIAL_BINDING_IDX, MATERIAL_UBO);
    return material;
}

