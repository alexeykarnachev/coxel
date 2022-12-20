#pragma once
#include "./components/gui_rect.h"
#include "./components/gui_text.h"
#include "./components/transformation.h"
#include "./editor_gui.h"
#include "bitset.h"
#include "constants.h"
#include "la.h"

typedef enum COMPONENT_TYPE {
    TRANSFORMATION_COMPONENT,
    MATERIAL_COMPONENT,
    MESH_COMPONENT,
    CAMERA_COMPONENT,
    POINT_LIGHT_COMPONENT,
    SCRIPT_COMPONENT,
    SPRITE_COMPONENT,
    HAS_OUTLINE_COMPONENT,

    GUI_TEXT_COMPONENT,
    GUI_RECT_COMPONENT,
    GUI_WIDGET_COMPONENT,

    N_COMPONENT_TYPES
} COMPONENT_TYPE;

typedef struct Entity {
    Bitset components;
    int parent_id;
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

size_t ecs_create_entity(int parent);
void ecs_add_component(int entity, COMPONENT_TYPE type, void* ptr);
void ecs_enable_component(int entity, COMPONENT_TYPE type);
void ecs_disable_component(int entity, COMPONENT_TYPE type);
int ecs_is_component_enabled(int entity, COMPONENT_TYPE type);
int ecs_get_parent_with_component(int entity, COMPONENT_TYPE type);
void ecs_set_tag(int entity, int tag);
int ecs_get_tag(int entity);
Mat4 ecs_get_world_mat(size_t entity);
Mat4 ecs_get_local_mat(size_t entity);
Vec3 ecs_get_world_position(size_t entity);
Mat4 ecs_get_origin_world_mat(size_t entity);
GUIRect* ecs_get_gui_rect(size_t entity);
GUIText* ecs_get_gui_text(size_t entity);
Transformation* ecs_get_transformation(size_t entity);
GUIWidget* ecs_get_gui_widget(size_t entity);
int ecs_get_active_camera_entity();
void ecs_update();
int ecs_check_if_camera(int entity);
int ecs_check_if_renderable(int entity);
int ecs_check_if_point_light(int entity);
int ecs_check_if_script(int entity);
int ecs_check_if_sprite(int entity);
int ecs_check_if_mesh_with_outline(int entity);
int ecs_check_if_sprite_with_outline(int entity);
int ecs_check_if_gui_rect(int entity);
int ecs_check_if_gui_text(int entity);
