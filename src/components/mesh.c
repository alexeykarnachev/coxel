#include "mesh.h"
#include "stddef.h"


static Mesh MESHES_ARENA[1 << 14];
static size_t N_MESHES = 0;

static VAOBuffer _PLANE_VAO_BUFFER;
static VAOBuffer _SPHERE_VAO_BUFFER;
static VAOBuffer _SURF_SPHERE_VAO_BUFFER;
static VAOBuffer _SUZANNE_VAO_BUFFER;
static VAOBuffer _IRONMAN_VAO_BUFFER;


Mesh* mesh_create(VAOBuffer vao_buffer) {
    Mesh* m = &MESHES_ARENA[N_MESHES++];

    m->vao_buffer = vao_buffer;

    return m;
}

Mesh* mesh_create_plane() {
    if (_PLANE_VAO_BUFFER.vao == 0) {
        vao_buffer_create_from_obj(
            &_PLANE_VAO_BUFFER, "./assets/meshes/plane.obj");
    }
    return mesh_create(_PLANE_VAO_BUFFER);
}

Mesh* mesh_create_sphere() {
    if (_SPHERE_VAO_BUFFER.vao == 0) {
        vao_buffer_create_from_obj(
            &_SPHERE_VAO_BUFFER, "./assets/meshes/sphere.obj");
    }
    return mesh_create(_SPHERE_VAO_BUFFER);
}

Mesh* mesh_create_surf_sphere() {
    if (_SURF_SPHERE_VAO_BUFFER.vao == 0) {
        vao_buffer_create_from_obj(
            &_SURF_SPHERE_VAO_BUFFER, "./assets/meshes/surf_sphere.obj");
    }
    return mesh_create(_SURF_SPHERE_VAO_BUFFER);
}

Mesh* mesh_create_suzanne() {
    if (_SUZANNE_VAO_BUFFER.vao == 0) {
        vao_buffer_create_from_obj(
            &_SUZANNE_VAO_BUFFER, "./assets/meshes/suzanne.obj");
    }
    return mesh_create(_SUZANNE_VAO_BUFFER);
}

Mesh* mesh_create_ironman() {
    if (_IRONMAN_VAO_BUFFER.vao == 0) {
        vao_buffer_create_from_obj(
            &_IRONMAN_VAO_BUFFER, "./assets/meshes/ironman.obj");
    }
    return mesh_create(_IRONMAN_VAO_BUFFER);
}

