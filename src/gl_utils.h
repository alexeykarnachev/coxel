void gl_create_rgba16f_buffer(GLuint* tex, size_t width, size_t height) {
    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_2D, *tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void gl_create_depth_buffer(GLuint* rbo, size_t width, size_t height) {
    glGenRenderbuffers(1, rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

void gl_create_depth_cubemap(GLuint* tex, size_t width, size_t height) {
    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *tex);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_DEPTH_COMPONENT,
                width,
                height,
                0,
                GL_DEPTH_COMPONENT,
                GL_FLOAT,
                NULL
       );
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
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
    const char* vert_file_path,
    const char* tesc_file_path,
    const char* tese_file_path,
    const char* geom_file_path,
    const char* frag_file_path,
    GLuint program,
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

