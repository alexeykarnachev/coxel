bool program_create_depth_cubemap(GLuint program);
bool program_create(
    GLuint program,
    const char* vert_file_path,
    const char* tesc_file_path,
    const char* tese_file_path,
    const char* geom_file_path,
    const char* frag_file_path,
    const int n_deps,
    const char* deps_file_paths[]
);
bool program_compile_file(
        const char* file_path,
        const char* deps_file_paths[],
        const size_t n_deps,
        GLenum shader_type,
        GLuint* shader
);
bool program_compile_source(const GLchar* sources[], size_t n_sources, GLenum shader_type, GLuint* shader);
bool program_get_attrib_location(GLuint program, GLuint* loc, const char* name);
bool program_get_uniform_location(GLuint program, GLuint* loc, const char* name);
bool program_set_attribute(GLuint program, const char* name, size_t n_components, GLenum type);
bool program_set_uniform_1i(GLuint program, const char* name, GLint val);
bool program_set_uniform_1f(GLuint program, const char* name, GLfloat val);
bool program_set_uniform_2f(GLuint program, const char* name, GLfloat v1, GLfloat v2);
bool program_set_uniform_3fv(GLuint program, const char* name, GLfloat* data, size_t n_vectors);
bool program_set_uniform_matrix_4fv(
        GLuint program, const char* name, GLfloat* data, size_t n_matrices, GLboolean transpose);

const char* VERT_PROJECTION_SHADER = "./assets/shaders/projection.vert";
const char* GEOM_CUBEMAP_SHADER = "./assets/shaders/cubemap.geom";
const char* FRAG_MATERIAL_SHADER = "./assets/shaders/material.frag";
const char* FRAG_DEPTH_SHADER = "./assets/shaders/depth.frag";
const char* GLSL_COMMON_SHADER = "./assets/shaders/common.glsl";
const char* VERSION_SHADER = "./assets/shaders/version.glsl";
const char* CONSTANTS_SHADER = "./src/constants.h";


bool program_create_depth_cubemap(GLuint program) {
    const char* deps_file_paths[] = {VERSION_SHADER, CONSTANTS_SHADER};
    return program_create(
        program, VERT_PROJECTION_SHADER, NULL, NULL, GEOM_CUBEMAP_SHADER, FRAG_DEPTH_SHADER, 2, deps_file_paths);
}

bool program_create(
    GLuint program,
    const char* vert_file_path,
    const char* tesc_file_path,
    const char* tese_file_path,
    const char* geom_file_path,
    const char* frag_file_path,
    const int n_deps,
    const char* deps_file_paths[]
) {
    GLuint vert_shader = 0;
    GLuint tesc_shader = 0;
    GLuint tese_shader = 0;
    GLuint geom_shader = 0;
    GLuint frag_shader = 0;

    bool is_compiled = true;

    is_compiled &= program_compile_file(vert_file_path, deps_file_paths, n_deps, GL_VERTEX_SHADER, &vert_shader);
    glAttachShader(program, vert_shader);

    is_compiled &= program_compile_file(frag_file_path, deps_file_paths, n_deps, GL_FRAGMENT_SHADER, &frag_shader);
    glAttachShader(program, frag_shader);

    if (tesc_file_path != NULL) {
        is_compiled &= program_compile_file(tesc_file_path, deps_file_paths, n_deps, GL_TESS_CONTROL_SHADER, &tesc_shader);
        glAttachShader(program, tesc_shader);
    }

    if (tese_file_path != NULL) {
        is_compiled &= program_compile_file(tese_file_path, deps_file_paths, n_deps, GL_TESS_EVALUATION_SHADER, &tese_shader);
        glAttachShader(program, tese_shader);
    }

    if (geom_file_path != NULL) {
        is_compiled &= program_compile_file(geom_file_path, deps_file_paths, n_deps, GL_GEOMETRY_SHADER, &geom_shader);
        glAttachShader(program, geom_shader);
    }

    if (!is_compiled) {
        fprintf(stderr, "ERROR: failed to compile some shader files\n");
        return false;
    }

    glLinkProgram(program);

    glDetachShader(program, vert_shader);
    glDetachShader(program, tesc_shader);
    glDetachShader(program, tese_shader);
    glDetachShader(program, geom_shader);
    glDetachShader(program, frag_shader);

    GLint is_linked;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
    if (is_linked != GL_TRUE) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetProgramInfoLog(program, sizeof(message), &message_size, message);
        fprintf(stderr, "ERROR: failed to link the program %s\n", message);
        return false;
    }

    return true;
}

bool program_compile_source(const GLchar* sources[], size_t n_sources, GLenum shader_type, GLuint* shader) {
    *shader = glCreateShader(shader_type);
    glShaderSource(*shader, n_sources, sources, NULL);
    glCompileShader(*shader);

    GLint is_compiled;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &is_compiled);
    if (is_compiled != GL_TRUE) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(*shader, sizeof(message), &message_size, message);
        fprintf(stderr, "ERROR: failed to compile the shader source %s\n", message);
        return false;
    }
    return true;
}

bool program_compile_file(
        const char* file_path,
        const char* deps_file_paths[],
        const size_t n_deps,
        GLenum shader_type,
        GLuint* shader
) {
    const char* sources[n_deps + 1];
    sources[n_deps] = read_cstr_file(file_path);
    if (sources[n_deps] == NULL) {
        fprintf(stderr, "ERROR: failed to read the shader source file `%s`: %s\n", file_path, strerror(errno));
        errno = 0;
        return false;
    }

    for (size_t i = 0; i < n_deps; ++i) {
        sources[i] = read_cstr_file(deps_file_paths[i]);
        if (sources[i] == NULL) {
            fprintf(stderr, "ERROR: failed to read the shader deps source file `%s`: %s\n", deps_file_paths[i], strerror(errno));
            errno = 0;
            return false;
        }
    }

    bool is_compiled = program_compile_source(sources, 1 + n_deps, shader_type, shader);

    for (size_t i = 0; i < 1 + n_deps; ++i) {
        free((char*)sources[i]);
    }

    if (!is_compiled) {
        fprintf(stderr, "ERROR: failed to compile the shader source file (or deps files) `%s`\n", file_path);
        return false;
    }

    return true;
}

bool program_get_attrib_location(GLuint program, GLuint* loc, const char* name) {
    GLuint _loc = glGetAttribLocation(program, name);
    *loc = _loc;

    if (_loc == -1) {
        #ifndef IGNORE_SHADER_LOC_ERRORS
        fprintf(stderr, "ERROR: failed to get the location of the attribute `%s`\n", name);
        return false;
        #endif
    }

    return true;
}

bool program_get_uniform_location(GLuint program, GLuint* loc, const char* name) {
    GLuint _loc = glGetUniformLocation(program, name);
    *loc = _loc;

    if (_loc == -1) {
        #ifndef IGNORE_SHADER_LOC_ERRORS
        fprintf(stderr, "ERROR: failed to get the location of the uniform `%s`\n", name);
        return false;
        #endif
    }

    return true;
}

bool program_set_attribute(GLuint program, const char* name, size_t n_components, GLenum type) {
    glUseProgram(program);

    GLuint loc;
    if (!program_get_attrib_location(program, &loc, name)) {
        return false;
    }

    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, n_components, type, GL_FALSE, 0, 0);

    return true;
}

#define _GET_UNIFORM_LOC \
    glUseProgram(program);\
    GLuint loc;\
    if (!program_get_uniform_location(program, &loc, name)) {\
        return false;\
    }\

bool program_set_uniform_1i(GLuint program, const char* name, GLint val) {
    _GET_UNIFORM_LOC
    glUniform1i(loc, val);
    return true;
}

bool program_set_uniform_1f(GLuint program, const char* name, GLfloat val) {
    _GET_UNIFORM_LOC
    glUniform1f(loc, val);
    return true;
}

bool program_set_uniform_1fv(GLuint program, const char* name, GLfloat* data, size_t n_values) {
    _GET_UNIFORM_LOC
    glUniform1fv(loc, n_values, data);
    return true;
}

bool program_set_uniform_2f(GLuint program, const char* name, GLfloat v1, GLfloat v2) {
    _GET_UNIFORM_LOC
    glUniform2f(loc, v1, v2);
    return true;
}

bool program_set_uniform_3fv(GLuint program, const char* name, GLfloat* data, size_t n_vectors) {
    _GET_UNIFORM_LOC
    glUniform3fv(loc, n_vectors, data);
    return true;
}

bool program_set_uniform_matrix_4fv(
        GLuint program, const char* name, GLfloat* data, size_t n_matrices, GLboolean transpose) {
    _GET_UNIFORM_LOC
    glUniformMatrix4fv(loc, n_matrices, transpose, data);
    return true;
}

