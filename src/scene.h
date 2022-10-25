char SCENE_COMPONENTS[MAX_N_SCENE_COMPONENTS];
size_t N_SCENE_COMPONENTS = 0;


void scene_add_component(size_t id) {
    SCENE_COMPONENTS[N_SCENE_COMPONENTS++] = id;
}
