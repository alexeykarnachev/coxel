#pragma once
#include "bitset.h"
#include "constants.h"
#include "la.h"

typedef enum COMPONENT_TYPE {
    TRANSFORMATION_T,
    MATERIAL_T,
    MESH_T,
    CAMERA_T,
    POINT_LIGHT_T,
    SCRIPT_T,
    SPRITE_T,
    HAS_OUTLINE_T,

    GUI_TEXT_T,
    GUI_RECT_T,

    N_COMPONENT_TYPES
} COMPONENT_TYPE;

typedef struct Entity {
    Bitset components;
    size_t parent_id;
    size_t id;
} Entity;

extern void* COMPONENTS[N_COMPONENT_TYPES][MAX_N_ENTITIES];

extern Entity ENTITIES[MAX_N_ENTITIES];
extern size_t N_ENTITIES;

// TODO: Put these arrays in one 2d array and index them via
// entity types (now I have only component types, so entity types need
// to be created). Entity type is a bundle of component types.
extern size_t RENDERABLE_ENTITIES[MAX_N_ENTITIES];
extern size_t CAMERA_ENTITIES[MAX_N_ENTITIES];
extern size_t POINT_LIGHT_ENTITIES[MAX_N_ENTITIES];
extern size_t SCRIPT_ENTITIES[MAX_N_ENTITIES];
extern size_t SPRITE_ENTITIES[MAX_N_ENTITIES];
extern size_t MESH_WITH_OUTLINE_ENTITIES[MAX_N_ENTITIES];
extern size_t SPRITE_WITH_OUTLINE_ENTITIES[MAX_N_ENTITIES];
extern size_t GUI_RECT_ENTITIES[MAX_N_ENTITIES];
extern size_t GUI_TEXT_ENTITIES[MAX_N_ENTITIES];

extern size_t N_RENDERABLE_ENTITIES;
extern size_t N_CAMERA_ENTITIES;
extern size_t N_POINT_LIGHT_ENTITIES;
extern size_t N_SCRIPT_ENTITIES;
extern size_t N_GUI_RECT_ENTITIES;
extern size_t N_GUI_TEXT_ENTITIES;
extern size_t N_SPRITE_ENTITIES;
extern size_t N_MESH_WITH_OUTLINE_ENTITIES;
extern size_t N_SPRITE_WITH_OUTLINE_ENTITIES;

size_t ecs_create_entity(size_t parent);
void ecs_add_component(size_t entity, COMPONENT_TYPE type, void* ptr);
void ecs_enable_component(size_t entity, COMPONENT_TYPE type);
void ecs_disable_component(size_t entity, COMPONENT_TYPE type);
int ecs_is_component_enabled(size_t entity, COMPONENT_TYPE type);
Mat4 ecs_get_world_mat(size_t entity);
Mat4 ecs_get_local_mat(size_t entity);
Vec3 ecs_get_world_position(size_t entity);
Mat4 ecs_get_origin_world_mat(size_t entity);
int ecs_get_active_camera_entity();
void ecs_update();
int ecs_check_if_camera(size_t entity);
int ecs_check_if_renderable(size_t entity);
int ecs_check_if_point_light(size_t entity);
int ecs_check_if_script(size_t entity);
int ecs_check_if_sprite(size_t entity);
int ecs_check_if_mesh_with_outline(size_t entity);
int ecs_check_if_sprite_with_outline(size_t entity);
int ecs_check_if_gui_rect(size_t entity);
int ecs_check_if_gui_text(size_t entity);
