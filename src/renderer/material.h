typedef struct Material {
    Vec3 diffuse_color;
    Vec3 ambient_color;
    Vec3 specular_color;
    float shininess;
} Material;

#define _MATERIAL_UBO_N_BYTES 52
#define _MATERIAL_UBO_N_ELEMENTS 13

void material_pack(Material* material, float dst[]) {
    size_t size = sizeof(float);
    memcpy(&dst[0], material->diffuse_color.data, size * 3);
    memcpy(&dst[4], material->ambient_color.data, size * 3);
    memcpy(&dst[8], material->specular_color.data, size * 3);
    dst[12] = material->shininess;
}

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

typedef struct MaterialUBO {
    GLuint ubo;
} MaterialUBO;


bool material_create_program(GLuint program) {
    const char* deps_file_paths[] = {VERSION_SHADER, CONSTANTS_SHADER, GLSL_COMMON_SHADER};
    return program_create(
        program, VERT_PROJECTION_SHADER, NULL, NULL, NULL, FRAG_MATERIAL_SHADER, 3, deps_file_paths);
}

bool material_create_ubo(MaterialUBO* material_ubo) {
    clear_struct(material_ubo);

    GLuint ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, _MATERIAL_UBO_N_BYTES, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    material_ubo->ubo = ubo;

    return true;
}

bool material_bind(Material* material, GLuint material_program, MaterialUBO* material_ubo) {
    GLuint idx = glGetUniformBlockIndex(material_program, "Material");
    if (idx == -1) {
        fprintf(stderr, "ERROR: can't find `Material` uniform block in the program");
        return false;
    }

    glUniformBlockBinding(material_program, idx, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, material_ubo->ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, material_ubo->ubo);

    static float data[_MATERIAL_UBO_N_ELEMENTS];
    material_pack(material, data);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, _MATERIAL_UBO_N_BYTES, data);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return true;
}


