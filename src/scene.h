typedef enum {
    ERROR_T,
    MESH_T,
    CAMERA_T,
    MATERIAL_T,
    TRANSFORMATION_T,
    POINT_LIGHT_T
} ComponentType;

typedef struct SceneComponent {
    ComponentType type;
    uint32_t gid;
    uint32_t lid;
} SceneComponent;

uint32_t _SCENE_CREATED = false;
int32_t _SCENE_ACTIVE_CAMERA_GID = -1;
uint32_t SCENE_N_COMPONENTS = 0;
uint32_t _N_MESHES = 0;
uint32_t _N_CAMERAS = 0;
uint32_t _N_MATERIALS= 0;
uint32_t _N_TRANSFORMATIONS = 0;
uint32_t _N_POINT_LIGHTS = 0;

SceneComponent SCENE_COMPONENTS[MAX_N_SCENE_COMPONENTS];
ArrayBuffer SCENE_MESH_BUFFERS[MAX_N_MESHES];
UBOStructsArray SCENE_CAMERA_BUFFERS;
UBOStructsArray SCENE_MATERIAL_BUFFERS;
UBOStructsArray SCENE_TRANSFORMATION_BUFFERS;
UBOStructsArray SCENE_POINT_LIGHT_BUFFERS;

#define _add_ubo_component(ptr, curr_n, max_n, pack_size, buffers, type_, pack_fn)\
    if (!_SCENE_CREATED) {\
        fprintf(stderr, "ERROR: can't add " #ptr ". Create a scene first\n");\
        return -1;\
    }\
    if (curr_n == max_n || SCENE_N_COMPONENTS == MAX_N_SCENE_COMPONENTS) {\
        fprintf(stderr, "ERROR: can't add " #ptr ". The maximum is reached\n");\
        return -1;\
    }\
    static float pack[pack_size];\
    pack_fn(ptr, pack);\
    ubo_structs_array_add(&buffers, pack);\
    SceneComponent* component = &SCENE_COMPONENTS[SCENE_N_COMPONENTS];\
    component->type = type_;\
    component->lid = curr_n++;\
    component->gid = SCENE_N_COMPONENTS++;\
    return component->gid;\


bool scene_create() {
    bool ok = true;
    ok &= ubo_structs_array_create(
        &SCENE_CAMERA_BUFFERS,
        MAX_N_CAMERAS,
        CAMERA_PACK_SIZE,
        CAMERA_BINDING_IDX,
        GL_DYNAMIC_DRAW
    );
    ok &= ubo_structs_array_create(
        &SCENE_MATERIAL_BUFFERS,
        MAX_N_MATERIALS,
        MATERIAL_PACK_SIZE,
        MATERIAL_BINDING_IDX,
        GL_STATIC_DRAW
    );
    ok &= ubo_structs_array_create(
        &SCENE_TRANSFORMATION_BUFFERS,
        MAX_N_TRANSFORMATIONS,
        TRANSFORMATION_PACK_SIZE,
        TRANSFORMATION_BINDING_IDX,
        GL_DYNAMIC_DRAW
    );
    ok &= ubo_structs_array_create(
        &SCENE_POINT_LIGHT_BUFFERS,
        MAX_N_POINT_LIGHTS,
        POINT_LIGHT_PACK_SIZE,
        POINT_LIGHT_BINDING_IDX,
        GL_STATIC_DRAW
    );

    _SCENE_CREATED = ok;
    return ok;
}

bool scene_set_active_camera_gid(uint32_t gid) {
    SceneComponent* component = &SCENE_COMPONENTS[gid];
    if (component->type != CAMERA_T) {
        fprintf(stderr, "ERROR: can't set scene camera. Passed gid doesn't belog to a camera");
        return false;
    }
    _SCENE_ACTIVE_CAMERA_GID = gid;
    return true;
}

int32_t scene_get_active_camera_lid() {
    if (_SCENE_ACTIVE_CAMERA_GID == -1) {
        return -1;
    }

    SceneComponent* component = &SCENE_COMPONENTS[_SCENE_ACTIVE_CAMERA_GID];
    return component->lid;
}

int scene_add_mesh(Mesh* mesh) {
    if (!_SCENE_CREATED) {
        fprintf(stderr, "ERROR: can't add mesh. Create a scene first\n");
        return -1;
    }

    if (_N_MESHES == MAX_N_MESHES || SCENE_N_COMPONENTS == MAX_N_SCENE_COMPONENTS) {
        fprintf(stderr, "ERROR: can't add mesh. The maximum is reached\n");
        return -1;
    }

    ArrayBuffer* b = &SCENE_MESH_BUFFERS[_N_MESHES];
    bool ok = array_buffer_create(
         b, mesh->faces, mesh->faces_size, mesh->vertices, mesh->vertices_size, GL_STATIC_DRAW);
    if (!ok) {
        fprintf(stderr, "ERROR: can't add mesh. Check array buffer creation error\n");
        return -1;
    }

    SceneComponent* component = &SCENE_COMPONENTS[SCENE_N_COMPONENTS];
    component->type = MESH_T;
    component->lid = _N_MESHES++;
    component->gid = SCENE_N_COMPONENTS++;

    return component->gid;
}

int scene_add_camera(Camera* camera) {
    _add_ubo_component(
        camera,
        _N_CAMERAS,
        MAX_N_CAMERAS,
        CAMERA_PACK_SIZE,
        SCENE_CAMERA_BUFFERS,
        CAMERA_T,
        camera_pack
    )
}

int scene_add_material(Material* material) {
    _add_ubo_component(
        material,
        _N_MATERIALS,
        MAX_N_MATERIALS,
        MATERIAL_PACK_SIZE,
        SCENE_MATERIAL_BUFFERS,
        MATERIAL_T,
        material_pack
    )
}

int scene_add_transformation(Transformation* transformation) {
    _add_ubo_component(
        transformation,
        _N_TRANSFORMATIONS,
        MAX_N_TRANSFORMATIONS,
        TRANSFORMATION_PACK_SIZE,
        SCENE_TRANSFORMATION_BUFFERS,
        TRANSFORMATION_T,
        transformation_pack
    )
}

int scene_add_point_light(PointLight* point_light) {
    _add_ubo_component(
        point_light,
        _N_POINT_LIGHTS,
        MAX_N_POINT_LIGHTS,
        POINT_LIGHT_PACK_SIZE,
        SCENE_POINT_LIGHT_BUFFERS,
        POINT_LIGHT_T,
        point_light_pack
    )
}

