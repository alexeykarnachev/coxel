#include "ecs.h"

#include "./components/transformation.h"

void* COMPONENTS[N_COMPONENT_TYPES][MAX_N_ENTITIES];

Entity ENTITIES[MAX_N_ENTITIES];
size_t N_ENTITIES = 0;

// TODO: Put these arrays in one 2d array and index them via
// entity types (now I have only component types, so entity types need
// to be created). Entity type is a bundle of component types.
size_t RENDERABLE_ENTITIES[MAX_N_ENTITIES];
size_t CAMERA_ENTITIES[MAX_N_ENTITIES];
size_t POINT_LIGHT_ENTITIES[MAX_N_ENTITIES];
size_t SCRIPT_ENTITIES[MAX_N_ENTITIES];
size_t SPRITE_ENTITIES[MAX_N_ENTITIES];
size_t MESH_WITH_OUTLINE_ENTITIES[MAX_N_ENTITIES];
size_t SPRITE_WITH_OUTLINE_ENTITIES[MAX_N_ENTITIES];
size_t GUI_RECT_ENTITIES[MAX_N_ENTITIES];
size_t GUI_TEXT_ENTITIES[MAX_N_ENTITIES];

size_t N_RENDERABLE_ENTITIES = 0;
size_t N_CAMERA_ENTITIES = 0;
size_t N_POINT_LIGHT_ENTITIES = 0;
size_t N_SCRIPT_ENTITIES = 0;
size_t N_GUI_RECT_ENTITIES = 0;
size_t N_GUI_TEXT_ENTITIES = 0;
size_t N_SPRITE_ENTITIES = 0;
size_t N_MESH_WITH_OUTLINE_ENTITIES = 0;
size_t N_SPRITE_WITH_OUTLINE_ENTITIES = 0;

void ecs_update() {
    // TODO: put these counters in one array and reset them at once.
    N_RENDERABLE_ENTITIES = 0;
    N_CAMERA_ENTITIES = 0;
    N_POINT_LIGHT_ENTITIES = 0;
    N_SCRIPT_ENTITIES = 0;
    N_SPRITE_ENTITIES = 0;
    N_MESH_WITH_OUTLINE_ENTITIES = 0;
    N_SPRITE_WITH_OUTLINE_ENTITIES = 0;
    N_GUI_RECT_ENTITIES = 0;
    N_GUI_TEXT_ENTITIES = 0;

    for (size_t entity = 0; entity < N_ENTITIES; ++entity) {
        if (ecs_check_if_renderable(entity)) {
            RENDERABLE_ENTITIES[N_RENDERABLE_ENTITIES++] = entity;
        }
        if (ecs_check_if_camera(entity)) {
            CAMERA_ENTITIES[N_CAMERA_ENTITIES++] = entity;
        }
        if (ecs_check_if_point_light(entity)) {
            POINT_LIGHT_ENTITIES[N_POINT_LIGHT_ENTITIES++] = entity;
        }
        if (ecs_check_if_script(entity)) {
            SCRIPT_ENTITIES[N_SCRIPT_ENTITIES++] = entity;
        }
        if (ecs_check_if_gui_rect(entity)) {
            GUI_RECT_ENTITIES[N_GUI_RECT_ENTITIES++] = entity;
        }
        if (ecs_check_if_gui_text(entity)) {
            GUI_TEXT_ENTITIES[N_GUI_TEXT_ENTITIES++] = entity;
        }
        if (ecs_check_if_sprite(entity)) {
            SPRITE_ENTITIES[N_SPRITE_ENTITIES++] = entity;
        }
        if (ecs_check_if_mesh_with_outline(entity)) {
            MESH_WITH_OUTLINE_ENTITIES[N_MESH_WITH_OUTLINE_ENTITIES++]
                = entity;
        }
        if (ecs_check_if_sprite_with_outline(entity)) {
            SPRITE_WITH_OUTLINE_ENTITIES[N_SPRITE_WITH_OUTLINE_ENTITIES++]
                = entity;
        }
    }
}

size_t ecs_create_entity(size_t parent) {
    Entity* entity = &ENTITIES[N_ENTITIES];
    size_t id = N_ENTITIES++;

    entity->id = id;
    entity->parent_id = parent;

    return id;
}

Mat4 ecs_get_world_mat(size_t entity) {
    Mat4 result = mat4_identity;

    do {
        Transformation* t = COMPONENTS[TRANSFORMATION_T][entity];
        entity = ENTITIES[entity].parent_id;
        Mat4 m = transformation_get_model_mat(t);
        m = mat4_transpose(&m);
        result = mat4_mat4_mul(&result, &m);
    } while (entity != -1);

    result = mat4_transpose(&result);
    return result;
}

Mat4 ecs_get_local_mat(size_t entity) {
    Transformation* t = COMPONENTS[TRANSFORMATION_T][entity];
    return transformation_get_model_mat(t);
}

Mat4 ecs_get_origin_world_mat(size_t entity) {
    size_t parent = ENTITIES[entity].parent_id;
    Mat4 world_mat;
    if (parent != -1) {
        world_mat = ecs_get_world_mat(parent);
    } else {
        world_mat = mat4_identity;
    }
    return world_mat;
}

Vec3 ecs_get_world_position(size_t entity) {
    Mat4 world_mat = ecs_get_world_mat(entity);
    Vec3 pos = vec3(
        world_mat.data[3], world_mat.data[7], world_mat.data[11]
    );
    return pos;
}

int ecs_get_active_camera_entity() {
    if (N_CAMERA_ENTITIES) {
        return CAMERA_ENTITIES[0];
    } else {
        return -1;
    }
}

void ecs_add_component(size_t entity, COMPONENT_TYPE type, void* ptr) {
    if (entity == -1)
        return;
    COMPONENTS[type][entity] = ptr;
    bitset_set_bit(&ENTITIES[entity].components, type);
}

void ecs_enable_component(size_t entity, COMPONENT_TYPE type) {
    if (entity == -1)
        return;
    bitset_set_bit(&ENTITIES[entity].components, type);
}

void ecs_disable_component(size_t entity, COMPONENT_TYPE type) {
    if (entity == -1)
        return;
    bitset_disable_bit(&ENTITIES[entity].components, type);
}

int ecs_check_if_renderable(size_t entity) {
    if (entity == -1)
        return 0;
    Bitset* b = &ENTITIES[entity].components;
    return bitset_get_bit(b, MESH_T) && bitset_get_bit(b, MATERIAL_T)
           && bitset_get_bit(b, TRANSFORMATION_T);
}

int ecs_check_if_camera(size_t entity) {
    if (entity == -1)
        return 0;
    Bitset* b = &ENTITIES[entity].components;
    return bitset_get_bit(b, CAMERA_T)
           && bitset_get_bit(b, TRANSFORMATION_T);
}

int ecs_check_if_point_light(size_t entity) {
    if (entity == -1)
        return 0;
    Bitset* b = &ENTITIES[entity].components;
    return bitset_get_bit(b, POINT_LIGHT_T)
           && bitset_get_bit(b, TRANSFORMATION_T);
}

int ecs_check_if_script(size_t entity) {
    if (entity == -1)
        return 0;
    Bitset* b = &ENTITIES[entity].components;
    return bitset_get_bit(b, SCRIPT_T);
}

int ecs_check_if_gui_rect(size_t entity) {
    if (entity == -1)
        return 0;
    Bitset* b = &ENTITIES[entity].components;
    return bitset_get_bit(b, GUI_RECT_T)
           && bitset_get_bit(b, TRANSFORMATION_T);
}

int ecs_check_if_gui_text(size_t entity) {
    if (entity == -1)
        return 0;
    Bitset* b = &ENTITIES[entity].components;
    return bitset_get_bit(b, GUI_TEXT_T)
           && bitset_get_bit(b, TRANSFORMATION_T);
}

int ecs_check_if_sprite(size_t entity) {
    if (entity == -1)
        return 0;
    Bitset* b = &ENTITIES[entity].components;
    return bitset_get_bit(b, SPRITE_T)
           && bitset_get_bit(b, TRANSFORMATION_T);
}

int ecs_check_if_mesh_with_outline(size_t entity) {
    if (entity == -1)
        return 0;
    Bitset* b = &ENTITIES[entity].components;
    return bitset_get_bit(b, HAS_OUTLINE_T) && bitset_get_bit(b, MESH_T)
           && bitset_get_bit(b, TRANSFORMATION_T);
}

int ecs_check_if_sprite_with_outline(size_t entity) {
    if (entity == -1)
        return 0;
    Bitset* b = &ENTITIES[entity].components;
    return bitset_get_bit(b, HAS_OUTLINE_T) && bitset_get_bit(b, SPRITE_T)
           && bitset_get_bit(b, TRANSFORMATION_T);
}
