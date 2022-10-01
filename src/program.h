typedef struct Program {
    GLuint gl_program;
} Program;

bool program_create(
    Program* program,
    const char* vert_file_path,
    const char* tesc_file_path,
    const char* tese_file_path,
    const char* geom_file_path,
    const char* frag_file_path,
    const int n_deps,
    const char* deps_file_paths[]
);

bool program_set_attribute(Program* program, const char* name, size_t n_components, GLenum type);
bool program_set_uniform_1i(Program* program, const char* name, GLint val);
bool program_set_uniform_1f(Program* program, const char* name, GLfloat val);
bool program_set_uniform_2f(Program* program, const char* name, GLfloat v1, GLfloat v2);
bool program_set_uniform_3fv(Program* program, const char* name, GLfloat* data, size_t n_vectors);
bool program_set_uniform_matrix_4fv(Program* program, const char* name, GLfloat* data, size_t n_matrices, GLboolean transpose);

bool program_create(
    Program* program,
    const char* vert_file_path,
    const char* tesc_file_path,
    const char* tese_file_path,
    const char* geom_file_path,
    const char* frag_file_path,
    const int n_deps,
    const char* deps_file_paths[]
) {
    memset(program, 0, sizeof(*program));

    GLuint gl_program = glCreateProgram();
    gl_link_program(
        vert_file_path,
        tesc_file_path,
        tese_file_path,
        geom_file_path,
        frag_file_path,
        gl_program,
        n_deps,
        deps_file_paths
    );

    program->gl_program = gl_program;
}

bool program_set_attribute(Program* program, const char* name, size_t n_components, GLenum type) {
    glUseProgram(program->gl_program);

    GLuint loc;
    if (!gl_get_program_attrib_location(&loc, program->gl_program, name)) {
        return false;
    }

    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, n_components, type, GL_FALSE, 0, 0);

    return true;
}


#define _GET_UNIFORM_LOC \
            glUseProgram(program->gl_program);\
            GLuint loc;\
            if (!gl_get_program_uniform_location(&loc, program->gl_program, name)) {\
                return false;\
            }\

bool program_set_uniform_1i(Program* program, const char* name, GLint val) {
    _GET_UNIFORM_LOC
    glUniform1i(loc, val);
    return true;
}

bool program_set_uniform_1f(Program* program, const char* name, GLfloat val) {
    _GET_UNIFORM_LOC
    glUniform1f(loc, val);
    return true;
}

bool program_set_uniform_2f(Program* program, const char* name, GLfloat v1, GLfloat v2) {
    _GET_UNIFORM_LOC
    glUniform2f(loc, v1, v2);
    return true;
}

bool program_set_uniform_3fv(Program* program, const char* name, GLfloat* data, size_t n_vectors) {
    _GET_UNIFORM_LOC
    glUniform3fv(loc, n_vectors, data);
    return true;
}

bool program_set_uniform_matrix_4fv(Program* program, const char* name, GLfloat* data, size_t n_matrices, GLboolean transpose) {
    _GET_UNIFORM_LOC
    glUniformMatrix4fv(loc, n_matrices, transpose, data);
    return true;
}

