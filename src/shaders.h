bool compile_shader_source(const GLchar* source, GLenum shader_type, GLuint* shader) {
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

bool compile_shader_file(const char* file_path, GLenum shader_type, GLuint* shader) {
    char* source = read_cstr_file(file_path);
    if (source == NULL) {
        fprintf(stderr, "ERROR: failed to read the shader source file `%s`: %s\n", file_path, strerror(errno));
        errno = 0;
        return false;
    }

    bool is_compiled = compile_shader_source(source, shader_type, shader);
    free(source);
    if (!is_compiled) {
        fprintf(stderr, "ERROR: failed to compile ther shader source file `%s`\n", file_path);
        return false;
    }

    return true;
}

bool link_program_files(
    const char* vert_file_path,
    const char* tesc_file_path,
    const char* tese_file_path,
    const char* frag_file_path,
    GLuint program
) {
    GLuint vert_shader = 0;
    GLuint tesc_shader = 0;
    GLuint tese_shader = 0;
    GLuint frag_shader = 0;

    bool is_compiled = true;
    is_compiled &= compile_shader_file(vert_file_path, GL_VERTEX_SHADER, &vert_shader);
    glAttachShader(program, vert_shader);

    is_compiled &= compile_shader_file(frag_file_path, GL_FRAGMENT_SHADER, &frag_shader);
    glAttachShader(program, frag_shader);

    if (tesc_file_path != NULL) {
        is_compiled &= compile_shader_file(tesc_file_path, GL_TESS_CONTROL_SHADER, &tesc_shader);
        glAttachShader(program, tesc_shader);
    }
    if (tese_file_path != NULL) {
        is_compiled &= compile_shader_file(tese_file_path, GL_TESS_EVALUATION_SHADER, &tese_shader);
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

bool get_attrib_location(GLuint* loc, GLuint program, const char* name) {
    GLuint _loc = glGetAttribLocation(program, name);
    if (_loc == -1) {
        fprintf(stderr, "ERROR: failed to get the location of the attribute `%s`\n", name);
        return false;
    }

    *loc = _loc;
    return true;
}

bool get_uniform_location(GLuint* loc, GLuint program, const char* name) {
    GLuint _loc = glGetUniformLocation(program, name);
    if (_loc == -1) {
        fprintf(stderr, "ERROR: failed to get the location of the uniform `%s`\n", name);
        return false;
    }

    *loc = _loc;
    return true;
}

