typedef enum COMPONENT_TYPE {
    TRANSFORMATION_T,
    MATERIAL_T,
    MESH_T,
    CAMERA_T,
    POINT_LIGHT_T,
    POINT_SHADOW_CASTER_T,
    HAS_POINT_SHADOW_T,
    GUI_TEXT_T,
    GUI_PANE_T,
    SCRIPT_T,
    SPRITE_T,

    N_COMPONENT_TYPES
} COMPONENT_TYPE;

typedef struct Entity {
    size_t id;
    Bitset components;
} Entity;

void* COMPONENTS[N_COMPONENT_TYPES][MAX_N_ENTITIES];

Entity ENTITIES[MAX_N_ENTITIES];
size_t N_ENTITIES = 0;

// TODO: Put these arrays in one 2d array and index them via
// entity types (now I have only component types, so entity types need
// to be created). Entity type is a bundle of component types.
size_t RENDERABLE_ENTITIES[MAX_N_ENTITIES];
size_t CAMERA_ENTITIES[MAX_N_ENTITIES];
size_t POINT_LIGHT_ENTITIES[MAX_N_ENTITIES];
size_t POINT_SHADOW_CASTER_ENTITIES[MAX_N_ENTITIES];
size_t HAS_POINT_SHADOW_ENTITIES[MAX_N_ENTITIES];
size_t SCRIPT_ENTITIES[MAX_N_ENTITIES];
size_t GUI_PANE_ENTITIES[MAX_N_ENTITIES];
size_t GUI_TEXT_ENTITIES[MAX_N_ENTITIES];
size_t SPRITE_ENTITIES[MAX_N_ENTITIES];

size_t N_RENDERABLE_ENTITIES = 0;
size_t N_CAMERA_ENTITIES = 0;
size_t N_POINT_LIGHT_ENTITIES = 0;
size_t N_POINT_SHADOW_CASTER_ENTITIES = 0;
size_t N_HAS_POINT_SHADOW_ENTITIES = 0;
size_t N_SCRIPT_ENTITIES = 0;
size_t N_GUI_PANE_ENTITIES = 0;
size_t N_GUI_TEXT_ENTITIES = 0;
size_t N_SPRITE_ENTITIES = 0;

void ecs_update();
size_t ecs_create_entity();
void ecs_add_component(size_t entity, COMPONENT_TYPE type, void* ptr);
int ecs_check_if_camera(size_t entity);
int ecs_check_if_renderable(size_t entity);
int ecs_check_if_point_light(size_t entity);
int ecs_check_if_point_shadow_caster(size_t entity);
int ecs_check_if_has_point_shadow(size_t entity);
int ecs_check_if_script(size_t entity);
int ecs_check_if_gui_pane(size_t entity);
int ecs_check_if_gui_text(size_t entity);
int ecs_check_if_sprite(size_t entity);


void ecs_update() {
    // TODO: put these counters in one array and reset them at once.
    N_RENDERABLE_ENTITIES = 0;
    N_CAMERA_ENTITIES = 0;
    N_POINT_LIGHT_ENTITIES = 0;
    N_POINT_SHADOW_CASTER_ENTITIES = 0;
    N_HAS_POINT_SHADOW_ENTITIES = 0;
    N_SCRIPT_ENTITIES = 0;
    N_GUI_PANE_ENTITIES = 0;
    N_GUI_TEXT_ENTITIES = 0;
    N_SPRITE_ENTITIES = 0;

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
        if (ecs_check_if_point_shadow_caster(entity)) {
            POINT_SHADOW_CASTER_ENTITIES[N_POINT_SHADOW_CASTER_ENTITIES++] = entity;
        }
        if (ecs_check_if_has_point_shadow(entity)) {
            HAS_POINT_SHADOW_ENTITIES[N_HAS_POINT_SHADOW_ENTITIES++] = entity;
        }
        if (ecs_check_if_script(entity)) {
            SCRIPT_ENTITIES[N_SCRIPT_ENTITIES++] = entity;
        }
        if (ecs_check_if_gui_pane(entity)) {
            GUI_PANE_ENTITIES[N_GUI_PANE_ENTITIES++] = entity;
        }
        if (ecs_check_if_gui_text(entity)) {
            GUI_TEXT_ENTITIES[N_GUI_TEXT_ENTITIES++] = entity;
        }
        if (ecs_check_if_sprite(entity)) {
            SPRITE_ENTITIES[N_SPRITE_ENTITIES++] = entity;
        }
    }
}

size_t ecs_create_entity() {
    Entity* entity = &ENTITIES[N_ENTITIES];
    entity->id = N_ENTITIES;
    return N_ENTITIES++;
}

void ecs_add_component(size_t entity, COMPONENT_TYPE type, void* ptr) {
    COMPONENTS[type][entity] = ptr; 
    bitset_set_bit(&ENTITIES[entity].components, type);
}

int ecs_check_if_renderable(size_t entity) {
    Bitset* b = &ENTITIES[entity].components;
    return
        bitset_get_bit(b, MESH_T)
        && bitset_get_bit(b, MATERIAL_T)
        && bitset_get_bit(b, TRANSFORMATION_T);
}

int ecs_check_if_camera(size_t entity) {
    Bitset* b = &ENTITIES[entity].components;
    return
        bitset_get_bit(b, CAMERA_T)
        && bitset_get_bit(b, TRANSFORMATION_T);
}

int ecs_check_if_point_light(size_t entity) {
    Bitset* b = &ENTITIES[entity].components;
    return
        bitset_get_bit(b, POINT_LIGHT_T)
        && bitset_get_bit(b, TRANSFORMATION_T);
}

int ecs_check_if_point_shadow_caster(size_t entity) {
    Bitset* b = &ENTITIES[entity].components;
    return
        bitset_get_bit(b, POINT_SHADOW_CASTER_T)
        && bitset_get_bit(b, TRANSFORMATION_T);
}

int ecs_check_if_has_point_shadow(size_t entity) {
    Bitset* b = &ENTITIES[entity].components;
    return
        bitset_get_bit(b, HAS_POINT_SHADOW_T)
        && ecs_check_if_renderable(entity);
}

int ecs_check_if_script(size_t entity) {
    Bitset* b = &ENTITIES[entity].components;
    return
        bitset_get_bit(b, SCRIPT_T);
}

int ecs_check_if_gui_pane(size_t entity) {
    Bitset* b = &ENTITIES[entity].components;
    return
        bitset_get_bit(b, GUI_PANE_T)
        && bitset_get_bit(b, TRANSFORMATION_T);
}

int ecs_check_if_gui_text(size_t entity) {
    Bitset* b = &ENTITIES[entity].components;
    return
        bitset_get_bit(b, GUI_TEXT_T)
        && bitset_get_bit(b, TRANSFORMATION_T);
}

int ecs_check_if_sprite(size_t entity) {
    Bitset* b = &ENTITIES[entity].components;
    return
        bitset_get_bit(b, SPRITE_T)
        && bitset_get_bit(b, TRANSFORMATION_T);
}
