bool shader_compile_source(const GLchar* source, GLenum shader_type, GLuint* shader) {
    *shader = glCreateShader(shader_type);
    glShaderSource(*shader, 1, &source, NULL);
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

bool shader_compile_file(const char* file_path, GLenum shader_type, GLuint* shader) {
    char* source = read_cstr_file(file_path);
    if (source == NULL) {
        fprintf(stderr, "ERROR: failed to read the shader source file `%s`: %s\n", file_path, strerror(errno));
        errno = 0;
        return false;
    }

    bool is_compiled = shader_compile_source(source, shader_type, shader);
    free(source);
    if (!is_compiled) {
        fprintf(stderr, "ERROR: failed to compile ther shader source file `%s`\n", file_path);
        return false;
    }

    return true;
}

bool shader_link_program(
    const char* vert_file_path,
    const char* tesc_file_path,
    const char* tese_file_path,
    const char* frag_file_path,
    GLuint program,
    const int n_deps,
    const char* deps_file_paths[]
) {
    GLuint vert_shader = 0;
    GLuint tesc_shader = 0;
    GLuint tese_shader = 0;
    GLuint frag_shader = 0;
    GLuint deps_shaders[n_deps];

    bool is_compiled = true;
    for (size_t i = 0; i < n_deps; ++i) {
        const char* deps_file_path = deps_file_paths[i];
        // TODO: chech shader type in runtime and put correct shader type here:
        is_compiled &= shader_compile_file(deps_file_path, GL_FRAGMENT_SHADER, &deps_shaders[i]);
        glAttachShader(program, deps_shaders[i]);
    }

    is_compiled &= shader_compile_file(vert_file_path, GL_VERTEX_SHADER, &vert_shader);
    glAttachShader(program, vert_shader);

    is_compiled &= shader_compile_file(frag_file_path, GL_FRAGMENT_SHADER, &frag_shader);
    glAttachShader(program, frag_shader);

    if (tesc_file_path != NULL) {
        is_compiled &= shader_compile_file(tesc_file_path, GL_TESS_CONTROL_SHADER, &tesc_shader);
        glAttachShader(program, tesc_shader);
    }
    if (tese_file_path != NULL) {
        is_compiled &= shader_compile_file(tese_file_path, GL_TESS_EVALUATION_SHADER, &tese_shader);
        glAttachShader(program, tese_shader);
    }

    if (!is_compiled) {
        fprintf(stderr, "ERROR: failed to compile some shader files\n");
        return false;
    }

    glLinkProgram(program);

    glDetachShader(program, vert_shader);
    glDetachShader(program, tesc_shader);
    glDetachShader(program, tese_shader);
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

bool shader_get_attrib_location(GLuint* loc, GLuint program, const char* name) {
    GLuint _loc = glGetAttribLocation(program, name);
    if (_loc == -1) {
        fprintf(stderr, "ERROR: failed to get the location of the attribute `%s`\n", name);
        return false;
    }

    *loc = _loc;
    return true;
}

bool shader_get_uniform_location(GLuint* loc, GLuint program, const char* name) {
    GLuint _loc = glGetUniformLocation(program, name);
    if (_loc == -1) {
        fprintf(stderr, "ERROR: failed to get the location of the uniform `%s`\n", name);
        return false;
    }

    *loc = _loc;
    return true;
}

