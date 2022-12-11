#include "program.h"

#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* VERT_SCREEN_RECT_SHADER
    = "./assets/shaders/screen_rect.vert";
static const char* VERT_PROJECTION_SHADER = "./assets/shaders/projection.vert";
static const char* VERT_GUI_RECT_SHADER = "./assets/shaders/gui_rect.vert";
static const char* VERT_GUI_TEXT_SHADER = "./assets/shaders/gui_text.vert";
static const char* VERT_SPRITE_SHADER = "./assets/shaders/sprite.vert";
static const char* VERT_SKYBOX_SHADER = "./assets/shaders/skybox.vert";
static const char* FRAG_COLOR_SHADER = "./assets/shaders/color.frag";
static const char* FRAG_DEPTH_SHADER = "./assets/shaders/depth.frag";
static const char* FRAG_GUI_RECT_SHADER = "./assets/shaders/gui_rect.frag";
static const char* FRAG_GUI_TEXT_SHADER = "./assets/shaders/gui_text.frag";
static const char* FRAG_GBUFFER_SHADER = "./assets/shaders/gbuffer.frag";
static const char* FRAG_OUTLINE_SHADER = "./assets/shaders/outline.frag";
static const char* FRAG_SPRITE_SHADER = "./assets/shaders/sprite.frag";
static const char* FRAG_SKYBOX_SHADER = "./assets/shaders/skybox.frag";
static const char* GLSL_COMMON_SHADER = "./assets/shaders/common.glsl";
static const char* VERSION_SHADER = "./assets/shaders/version.glsl";
static const char* CONSTANTS_SHADER = "./src/constants.h";

GLuint PROGRAM_COLOR;
GLuint PROGRAM_GUI_RECT;
GLuint PROGRAM_GUI_TEXT;
GLuint PROGRAM_MESH_GBUFFER;
GLuint PROGRAM_GUI_GBUFFER;
GLuint PROGRAM_SPRITE_GBUFFER;
GLuint PROGRAM_SKYBOX_GBUFFER;
GLuint PROGRAM_MESH_OUTLINE;
GLuint PROGRAM_SPRITE_OUTLINE;

static bool program_compile_source(
    const GLchar* sources[],
    size_t n_sources,
    GLenum shader_type,
    GLuint* shader
) {
    *shader = glCreateShader(shader_type);
    glShaderSource(*shader, n_sources, sources, NULL);
    glCompileShader(*shader);

    GLint is_compiled;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &is_compiled);
    if (is_compiled != GL_TRUE) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(*shader, sizeof(message), &message_size, message);
        fprintf(
            stderr, "ERROR: failed to compile the shader source %s\n", message
        );
        return false;
    }
    return true;
}

static bool program_compile_file(
    const char* file_path,
    const char* deps_file_paths[],
    const size_t n_deps,
    GLenum shader_type,
    GLuint* shader
) {
    const char* sources[n_deps + 1];
    sources[n_deps] = read_cstr_file(file_path, "r", NULL);
    if (sources[n_deps] == NULL) {
        fprintf(
            stderr,
            "ERROR: failed to read the shader source file `%s`: %s\n",
            file_path,
            strerror(errno)
        );
        errno = 0;
        return false;
    }

    for (size_t i = 0; i < n_deps; ++i) {
        sources[i] = read_cstr_file(deps_file_paths[i], "r", NULL);
        if (sources[i] == NULL) {
            fprintf(
                stderr,
                "ERROR: failed to read the shader deps source file `%s`: %s\n",
                deps_file_paths[i],
                strerror(errno)
            );
            errno = 0;
            return false;
        }
    }

    bool is_compiled
        = program_compile_source(sources, 1 + n_deps, shader_type, shader);

    for (size_t i = 0; i < 1 + n_deps; ++i) {
        free((char*)sources[i]);
    }

    if (!is_compiled) {
        fprintf(
            stderr,
            "ERROR: failed to compile the shader source file (or deps files) "
            "`%s`\n",
            file_path
        );
        return false;
    }

    return true;
}

static bool program_create(
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

    is_compiled &= program_compile_file(
        vert_file_path, deps_file_paths, n_deps, GL_VERTEX_SHADER, &vert_shader
    );
    glAttachShader(program, vert_shader);

    is_compiled &= program_compile_file(
        frag_file_path,
        deps_file_paths,
        n_deps,
        GL_FRAGMENT_SHADER,
        &frag_shader
    );
    glAttachShader(program, frag_shader);

    if (tesc_file_path != NULL) {
        is_compiled &= program_compile_file(
            tesc_file_path,
            deps_file_paths,
            n_deps,
            GL_TESS_CONTROL_SHADER,
            &tesc_shader
        );
        glAttachShader(program, tesc_shader);
    }

    if (tese_file_path != NULL) {
        is_compiled &= program_compile_file(
            tese_file_path,
            deps_file_paths,
            n_deps,
            GL_TESS_EVALUATION_SHADER,
            &tese_shader
        );
        glAttachShader(program, tese_shader);
    }

    if (geom_file_path != NULL) {
        is_compiled &= program_compile_file(
            geom_file_path,
            deps_file_paths,
            n_deps,
            GL_GEOMETRY_SHADER,
            &geom_shader
        );
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

bool program_create_all() {
    const char* deps_file_paths[]
        = {VERSION_SHADER, CONSTANTS_SHADER, GLSL_COMMON_SHADER};
    bool ok = true;

    PROGRAM_COLOR = glCreateProgram();
    ok &= program_create(
        PROGRAM_COLOR,
        VERT_SCREEN_RECT_SHADER,
        NULL,
        NULL,
        NULL,
        FRAG_COLOR_SHADER,
        3,
        deps_file_paths
    );

    PROGRAM_MESH_GBUFFER = glCreateProgram();
    ok &= program_create(
        PROGRAM_MESH_GBUFFER,
        VERT_PROJECTION_SHADER,
        NULL,
        NULL,
        NULL,
        FRAG_GBUFFER_SHADER,
        2,
        deps_file_paths
    );

    PROGRAM_GUI_GBUFFER = glCreateProgram();
    ok &= program_create(
        PROGRAM_GUI_GBUFFER,
        VERT_GUI_RECT_SHADER,
        NULL,
        NULL,
        NULL,
        FRAG_GBUFFER_SHADER,
        2,
        deps_file_paths
    );

    PROGRAM_SPRITE_GBUFFER = glCreateProgram();
    ok &= program_create(
        PROGRAM_SPRITE_GBUFFER,
        VERT_SPRITE_SHADER,
        NULL,
        NULL,
        NULL,
        FRAG_SPRITE_SHADER,
        2,
        deps_file_paths
    );

    PROGRAM_SKYBOX_GBUFFER = glCreateProgram();
    ok &= program_create(
        PROGRAM_SKYBOX_GBUFFER,
        VERT_SKYBOX_SHADER,
        NULL,
        NULL,
        NULL,
        FRAG_SKYBOX_SHADER,
        2,
        deps_file_paths
    );

    PROGRAM_MESH_OUTLINE = glCreateProgram();
    ok &= program_create(
        PROGRAM_MESH_OUTLINE,
        VERT_PROJECTION_SHADER,
        NULL,
        NULL,
        NULL,
        FRAG_OUTLINE_SHADER,
        2,
        deps_file_paths
    );

    PROGRAM_SPRITE_OUTLINE = glCreateProgram();
    ok &= program_create(
        PROGRAM_SPRITE_OUTLINE,
        VERT_SPRITE_SHADER,
        NULL,
        NULL,
        NULL,
        FRAG_OUTLINE_SHADER,
        2,
        deps_file_paths
    );

    PROGRAM_GUI_RECT = glCreateProgram();
    ok &= program_create(
        PROGRAM_GUI_RECT,
        VERT_GUI_RECT_SHADER,
        NULL,
        NULL,
        NULL,
        FRAG_GUI_RECT_SHADER,
        2,
        deps_file_paths
    );

    PROGRAM_GUI_TEXT = glCreateProgram();
    ok &= program_create(
        PROGRAM_GUI_TEXT,
        VERT_GUI_TEXT_SHADER,
        NULL,
        NULL,
        NULL,
        FRAG_GUI_TEXT_SHADER,
        2,
        deps_file_paths
    );

    return ok;
}

bool program_get_attrib_location(
    GLuint program, GLuint* loc, const char* name
) {
    GLuint _loc = glGetAttribLocation(program, name);
    *loc = _loc;

    if (_loc == -1) {
#ifndef IGNORE_SHADER_LOC_ERRORS
        fprintf(
            stderr,
            "ERROR: failed to get the location of the attribute `%s`\n",
            name
        );
        return false;
#endif
    }

    return true;
}

bool program_get_uniform_location(
    GLuint program, GLuint* loc, const char* name
) {
    GLuint _loc = glGetUniformLocation(program, name);
    *loc = _loc;

    if (_loc == -1) {
#ifndef IGNORE_SHADER_LOC_ERRORS
        fprintf(
            stderr,
            "ERROR: failed to get the location of the uniform `%s`\n",
            name
        );
        return false;
#endif
    }

    return true;
}

bool program_set_attribute(
    GLuint program, const char* name, size_t n_components, GLenum type
) {
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
    glUseProgram(program); \
    GLuint loc; \
    if (!program_get_uniform_location(program, &loc, name)) { \
        return false; \
    }

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

bool program_set_uniform_1fv(
    GLuint program, const char* name, GLfloat* data, size_t n_values
) {
    _GET_UNIFORM_LOC
    glUniform1fv(loc, n_values, data);
    return true;
}

bool program_set_uniform_1iv(
    GLuint program, const char* name, GLint* data, size_t n_values
) {
    _GET_UNIFORM_LOC
    glUniform1iv(loc, n_values, data);
    return true;
}

bool program_set_uniform_1uiv(
    GLuint program, const char* name, GLint* data, size_t n_values
) {
    _GET_UNIFORM_LOC
    glUniform1uiv(loc, n_values, data);
    return true;
}

bool program_set_uniform_2f(
    GLuint program, const char* name, GLfloat v1, GLfloat v2
) {
    _GET_UNIFORM_LOC
    glUniform2f(loc, v1, v2);
    return true;
}

bool program_set_uniform_3fv(
    GLuint program, const char* name, GLfloat* data, size_t n_vectors
) {
    _GET_UNIFORM_LOC
    glUniform3fv(loc, n_vectors, data);
    return true;
}

bool program_set_uniform_4fv(
    GLuint program, const char* name, GLfloat* data, size_t n_vectors
) {
    _GET_UNIFORM_LOC
    glUniform4fv(loc, n_vectors, data);
    return true;
}

bool program_set_uniform_matrix_4fv(
    GLuint program,
    const char* name,
    GLfloat* data,
    size_t n_matrices,
    GLboolean transpose
) {
    _GET_UNIFORM_LOC
    glUniformMatrix4fv(loc, n_matrices, transpose, data);
    return true;
}
