#pragma once
#include "../buffers/vao_buffer.h"

typedef struct Mesh {
    VAOBuffer vao_buffer;
} Mesh;

Mesh* mesh_create(VAOBuffer vao_buffer);
Mesh* mesh_create_plane();
Mesh* mesh_create_sphere();
Mesh* mesh_create_surf_sphere();
Mesh* mesh_create_suzanne();
Mesh* mesh_create_ironman();
