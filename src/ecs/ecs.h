enum COMPONENT_TYPE {
    TRANSFORMATION_T,
    MATERIAL_T,
    MESH_T,
    CAMERA_T,
    SPRITE_T,
    POINT_LIGHT_T,
    POINT_SHADOW_CASTER_T,
    GUI_TEXT_T,
    GUI_PANE_T,
    SCRIPT_T,

    N_COMPONENT_TYPES
};

typedef struct Entity {
    size_t id;
    Bitset components;
} Entity;

void* COMPONENTS[N_COMPONENT_TYPES][MAX_N_ENTITIES];

_Entity ENTITIES[MAX_N_ENTITIES];
size_t N_ENTITIES = 0;

size_t RENDERABLE_ENTITIES[MAX_N_ENTITIES];
size_t CAMERA_ENTITIES[MAX_N_ENTITIES];
size_t POINT_LIGHT_ENTITIES[MAX_N_ENTITIES]

size_t N_RENDERABLE_ENTITIES = 0;
size_t N_CAMERA_ENTITIES = 0;
size_t N_POINT_LIGHT_ENTITIES = 0;

void ecs_update();
size_t ecs_create_entity();
void ecs_add_component(size_t entity, COMPONENT_TYPE type, void* ptr);
void ecs_check_if_camera(size_t entity);
void ecs_check_if_renderable(size_t entity);
void ecs_check_if_point_ligth(size_t entity);


void ecs_update() {
    N_RENDERABLE_ENTITIES = 0;
    N_CAMERA_ENTITIES = 0;
    N_POINT_LIGHT_ENTITIES = 0;

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
    }
}

size_t ecs_create_entity() {
    Entity* entity = ENTITIES[N_ENTITIES];
    entity->id = N_ENTITIES;
    return N_ENTITIES++;
}

void ecs_add_component(size_t entity, COMPONENT_TYPE type, void* ptr) {
    COMPONENTS[type][entity] = ptr; 
    bitset_set_bit(ENTITIES[entity].components, type);
}

void ecs_check_if_renderable(size_t entity) {
    Bitset* b = ENTITIES[entity].components;
    return
        bitset_get_bit(b, MESH_T)
        && bitset_get_bit(b, MATERIAL_T)
        && bitset_get_bit(b, TRANSFORMATION_T)
}

void ecs_check_if_camera(size_t entity) {
    Bitset* b = ENTITIES[entity].components;
    return
        bitset_get_bit(b, CAMERA_T)
        && bitset_get_bit(b, TRANSFORMATION_T)
}

void ecs_check_if_point_ligth(size_t entity) {
    Bitset* b = ENTITIES[entity].components;
    return
        bitset_get_bit(b, POINT_LIGHT_T)
        && bitset_get_bit(b, TRANSFORMATION_T)
}

