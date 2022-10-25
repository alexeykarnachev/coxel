typedef enum {
    ERROR_T
    CAMERA_T
} ComponentType;

typedef struct SceneComponent {
    ComponentType type;
    size_t id;
} SceneComponent;

SceneComponent SCENE_COMPONENTS[MAX_N_SCENE_COMPONENTS];
size_t SCENE_N_COMPONENTS = 0;
size_t SCENE_N_CAMERAS = 0;


SceneComponent scene_add_camera(Camera) {
    if (SCENE_N_CAMERAS == MAX_N_CAMERAS) {
        fprintf(stderr, "ERROR: can't create any more cameras. The maximum is reached")
        return {ERROR_T, 0};
    }
    SceneComponent c = {CAMERA_T, SCENE_N_CAMERAS++};
    SCENE_COMPONENTS[SCENE_N_COMPONENTS++] = c;
    return c;
}
