void gl_create_depth_rbo(GLuint* rbo, size_t width, size_t height);
void gl_create_rgba16f_tex(GLuint* tex, size_t width, size_t height);
bool gl_create_fbo_with_2d_depth_tex(GLuint* fbo, GLuint* tex, size_t width, size_t height);
bool gl_create_fbo_with_cube_tex(GLuint* fbo, GLuint* tex, size_t width, size_t height);

bool gl_link_program(
    GLuint program,
    const char* vert_file_path,
    const char* tesc_file_path,
    const char* tese_file_path,
    const char* geom_file_path,
    const char* frag_file_path,
    const int n_deps,
    const char* deps_file_paths[]
);
bool gl_compile_shader_file(
        const char* file_path,
        const char* deps_file_paths[],
        const size_t n_deps,
        GLenum shader_type,
        GLuint* shader
);

bool gl_compile_shader_source(const GLchar* sources[], size_t n_sources, GLenum shader_type, GLuint* shader);
bool gl_get_program_attrib_location(GLuint* loc, GLuint program, const char* name);
bool gl_get_program_uniform_location(GLuint* loc, GLuint program, const char* name);
bool gl_set_program_attribute(GLuint program, const char* name, size_t n_components, GLenum type);
bool gl_set_program_uniform_1i(GLuint program, const char* name, GLint val);
bool gl_set_program_uniform_1f(GLuint program, const char* name, GLfloat val);
bool gl_set_program_uniform_2f(GLuint program, const char* name, GLfloat v1, GLfloat v2);
bool gl_set_program_uniform_3fv(GLuint program, const char* name, GLfloat* data, size_t n_vectors);
bool gl_set_program_uniform_matrix_4fv(GLuint program, const char* name, GLfloat* data, size_t n_matrices, GLboolean transpose);

#define _CHECK_FRAMEBUFFER \
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {\
                fprintf(stderr, "ERROR: framebuffer not complete\n");\
                return false;\
            }\

bool gl_create_fbo_with_2d_depth_tex(GLuint* fbo, GLuint* tex, size_t width, size_t height) {
    glGenFramebuffers(1, fbo);
    glGenTextures(1, tex);
    
    glBindTexture(GL_TEXTURE_2D, *tex);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *tex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    _CHECK_FRAMEBUFFER

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool gl_create_fbo_with_cube_tex(GLuint* fbo, GLuint* tex, size_t width, size_t height) {
    glGenFramebuffers(1, fbo);
    glGenTextures(1, tex);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, *tex);
    for (size_t i = 0; i < 6; ++i) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *tex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    _CHECK_FRAMEBUFFER

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void gl_create_depth_rbo(GLuint* rbo, size_t width, size_t height) {
    glGenRenderbuffers(1, rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

void gl_create_rgba16f_tex(GLuint* tex, size_t width, size_t height) {
    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_2D, *tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

bool gl_compile_shader_source(const GLchar* sources[], size_t n_sources, GLenum shader_type, GLuint* shader) {
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

bool gl_compile_shader_file(
        const char* file_path,
        const char* deps_file_paths[],
        const size_t n_deps,
        GLenum shader_type,
        GLuint* shader
) {
    const char* sources[n_deps + 1];
    sources[0] = read_cstr_file(file_path);
    if (sources[0] == NULL) {
        fprintf(stderr, "ERROR: failed to read the shader source file `%s`: %s\n", file_path, strerror(errno));
        errno = 0;
        return false;
    }

    for (size_t i = 0; i < n_deps; ++i) {
        sources[i + 1] = read_cstr_file(deps_file_paths[i]);
        if (sources[i + 1] == NULL) {
            fprintf(stderr, "ERROR: failed to read the shader deps source file `%s`: %s\n", deps_file_paths[i], strerror(errno));
            errno = 0;
            return false;
        }
    }

    bool is_compiled = gl_compile_shader_source(sources, 1 + n_deps, shader_type, shader);

    for (size_t i = 0; i < 1 + n_deps; ++i) {
        free((char*)sources[i]);
    }

    if (!is_compiled) {
        fprintf(stderr, "ERROR: failed to compile the shader source file (or deps files) `%s`\n", file_path);
        return false;
    }

    return true;
}

bool gl_link_program(
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
    GLuint deps_shaders[n_deps];

    bool is_compiled = true;

    is_compiled &= gl_compile_shader_file(vert_file_path, deps_file_paths, n_deps, GL_VERTEX_SHADER, &vert_shader);
    glAttachShader(program, vert_shader);

    is_compiled &= gl_compile_shader_file(frag_file_path, deps_file_paths, n_deps, GL_FRAGMENT_SHADER, &frag_shader);
    glAttachShader(program, frag_shader);

    if (tesc_file_path != NULL) {
        is_compiled &= gl_compile_shader_file(tesc_file_path, deps_file_paths, n_deps, GL_TESS_CONTROL_SHADER, &tesc_shader);
        glAttachShader(program, tesc_shader);
    }

    if (tese_file_path != NULL) {
        is_compiled &= gl_compile_shader_file(tese_file_path, deps_file_paths, n_deps, GL_TESS_EVALUATION_SHADER, &tese_shader);
        glAttachShader(program, tese_shader);
    }

    if (geom_file_path != NULL) {
        is_compiled &= gl_compile_shader_file(geom_file_path, deps_file_paths, n_deps, GL_GEOMETRY_SHADER, &geom_shader);
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
    for (size_t i = 0; i < n_deps; ++i) {
        glDetachShader(program, deps_shaders[i]);
    }

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

bool gl_get_program_attrib_location(GLuint* loc, GLuint program, const char* name) {
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

bool gl_get_program_uniform_location(GLuint* loc, GLuint program, const char* name) {
    GLuint _loc = glGetUniformLocation(program, name);
    *loc = _loc;

    if (_loc == -1) {
        #ifndef IGNORE_SHADER_LOC_ERRORS
        fprintf(stderr, "ERROR: failed to get the location of the attribute `%s`\n", name);
        return false;
        #endif
    }

    return true;
}

bool gl_set_program_attribute(GLuint program, const char* name, size_t n_components, GLenum type) {
    glUseProgram(program);

    GLuint loc;
    if (!gl_get_program_attrib_location(&loc, program, name)) {
        return false;
    }

    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, n_components, type, GL_FALSE, 0, 0);

    return true;
}

#define _GET_UNIFORM_LOC \
            glUseProgram(program);\
            GLuint loc;\
            if (!gl_get_program_uniform_location(&loc, program, name)) {\
                return false;\
            }\

bool gl_set_program_uniform_1i(GLuint program, const char* name, GLint val) {
    _GET_UNIFORM_LOC
    glUniform1i(loc, val);
    return true;
}

bool gl_set_program_uniform_1f(GLuint program, const char* name, GLfloat val) {
    _GET_UNIFORM_LOC
    glUniform1f(loc, val);
    return true;
}

bool gl_set_program_uniform_2f(GLuint program, const char* name, GLfloat v1, GLfloat v2) {
    _GET_UNIFORM_LOC
    glUniform2f(loc, v1, v2);
    return true;
}

bool gl_set_program_uniform_3fv(GLuint program, const char* name, GLfloat* data, size_t n_vectors) {
    _GET_UNIFORM_LOC
    glUniform3fv(loc, n_vectors, data);
    return true;
}

bool gl_set_program_uniform_matrix_4fv(GLuint program, const char* name, GLfloat* data, size_t n_matrices, GLboolean transpose) {
    _GET_UNIFORM_LOC
    glUniformMatrix4fv(loc, n_matrices, transpose, data);
    return true;
}

