#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <glad/glad.h>

bool program_create_all();
bool program_get_attrib_location(GLuint program, GLuint* loc, const char* name);
bool program_get_uniform_location(GLuint program, GLuint* loc, const char* name);
bool program_set_attribute(GLuint program, const char* name, size_t n_components, GLenum type);
bool program_set_uniform_1i(GLuint program, const char* name, GLint val);
bool program_set_uniform_1f(GLuint program, const char* name, GLfloat val);
bool program_set_uniform_1fv(GLuint program, const char* name, GLfloat* data, size_t n_values);
bool program_set_uniform_1iv(GLuint program, const char* name, GLint* data, size_t n_values);
bool program_set_uniform_1uiv(GLuint program, const char* name, GLint* data, size_t n_values);
bool program_set_uniform_2f(GLuint program, const char* name, GLfloat v1, GLfloat v2);
bool program_set_uniform_3fv(GLuint program, const char* name, GLfloat* data, size_t n_vectors);
bool program_set_uniform_4fv(GLuint program, const char* name, GLfloat* data, size_t n_vectors);
bool program_set_uniform_matrix_4fv(
        GLuint program, const char* name, GLfloat* data, size_t n_matrices, GLboolean transpose);

extern GLuint PROGRAM_COLOR;
extern GLuint PROGRAM_GUI_RECT;
extern GLuint PROGRAM_GUI_TEXT;
extern GLuint PROGRAM_MESH_GBUFFER;
extern GLuint PROGRAM_GUI_GBUFFER;
extern GLuint PROGRAM_SPRITE_GBUFFER;
extern GLuint PROGRAM_SKYBOX_GBUFFER;
extern GLuint PROGRAM_MESH_OUTLINE;
extern GLuint PROGRAM_SPRITE_OUTLINE;

