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
    uint32_t global_id;
    uint32_t local_id;
} SceneComponent;

uint32_t _SCENE_CREATED = false;
int32_t SCENE_ACTIVE_CAMERA = -1;
uint32_t SCENE_N_COMPONENTS = 0;
uint32_t _N_MESHES = 0;
uint32_t _N_CAMERAS = 0;
uint32_t _N_MATERIALS= 0;
uint32_t _N_TRANSFORMATIONS = 0;
uint32_t _NPOINT_LIGHTS = 0;

SceneComponent SCENE_COMPONENTS[MAX_N_SCENE_COMPONENTS];
ArrayBuffer SCENE_MESH_BUFFERS[MAX_N_MESHES];
UBOStructsArray SCENE_CAMERA_BUFFERS;
UBOStructsArray SCENE_MATERIAL_BUFFERS;
UBOStructsArray SCENE_TRANSFORMATION_BUFFERS;
UBOStructsArray SCENE_POINT_LIGHT_BUFFERS;


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
    component->local_id = _N_MESHES++;
    component->global_id = SCENE_N_COMPONENTS++;

    return component->global_id;
}

int scene_add_camera(Camera* camera) {
    if (!_SCENE_CREATED) {
        fprintf(stderr, "ERROR: can't add camera. Create a scene first\n");
        return -1;
    }

    if (_N_CAMERAS == MAX_N_CAMERAS || SCENE_N_COMPONENTS == MAX_N_SCENE_COMPONENTS) {
        fprintf(stderr, "ERROR: can't add camera. The maximum is reached\n");
        return -1;
    }

    static float pack[CAMERA_PACK_SIZE];
    camera_pack(camera, pack);
    ubo_structs_array_add(&SCENE_CAMERA_BUFFERS, pack);

    SceneComponent* component = &SCENE_COMPONENTS[SCENE_N_COMPONENTS];
    component->type = CAMERA_T;
    component->local_id = _N_CAMERAS++;
    component->global_id = SCENE_N_COMPONENTS++;

    SCENE_ACTIVE_CAMERA = component->local_id;
    return component->global_id;
}

int scene_add_material(Material* material) {
    if (!_SCENE_CREATED) {
        fprintf(stderr, "ERROR: can't add material. Create a scene first\n");
        return -1;
    }

    if (_N_MATERIALS == MAX_N_MATERIALS || SCENE_N_COMPONENTS == MAX_N_SCENE_COMPONENTS) {
        fprintf(stderr, "ERROR: can't add material. The maximum is reached\n");
        return -1;
    }

    static float pack[MATERIAL_PACK_SIZE];
    material_pack(material, pack);
    ubo_structs_array_add(&SCENE_MATERIAL_BUFFERS, pack);

    SceneComponent* component = &SCENE_COMPONENTS[SCENE_N_COMPONENTS];
    component->type = MATERIAL_T;
    component->local_id = _N_MATERIALS++;
    component->global_id = SCENE_N_COMPONENTS++;

    return component->global_id;
}

int scene_add_transformation(Transformation* transformation) {
    if (!_SCENE_CREATED) {
        fprintf(stderr, "ERROR: can't add transformation. Create a scene first\n");
        return -1;
    }

    if (_N_TRANSFORMATIONS == MAX_N_TRANSFORMATIONS || SCENE_N_COMPONENTS == MAX_N_SCENE_COMPONENTS) {
        fprintf(stderr, "ERROR: can't add transformation. The maximum is reached\n");
        return -1;
    }

    static float pack[TRANSFORMATION_PACK_SIZE];
    transformation_pack(transformation, pack);
    ubo_structs_array_add(&SCENE_TRANSFORMATION_BUFFERS, pack);

    SceneComponent* component = &SCENE_COMPONENTS[SCENE_N_COMPONENTS];
    component->type = TRANSFORMATION_T;
    component->local_id = _N_TRANSFORMATIONS++;
    component->global_id = SCENE_N_COMPONENTS++;

    return component->global_id;
}

int scene_add_point_light(PointLight* point_light) {
    if (!_SCENE_CREATED) {
        fprintf(stderr, "ERROR: can't add point light. Create a scene first\n");
        return -1;
    }

    if (_NPOINT_LIGHTS == MAX_N_POINT_LIGHTS || SCENE_N_COMPONENTS == MAX_N_SCENE_COMPONENTS) {
        fprintf(stderr, "ERROR: can't add point light. The maximum is reached\n");
        return -1;
    }

    static float pack[POINT_LIGHT_PACK_SIZE];
    point_light_pack(point_light, pack);
    ubo_structs_array_add(&SCENE_POINT_LIGHT_BUFFERS, pack);

    SceneComponent* component = &SCENE_COMPONENTS[SCENE_N_COMPONENTS];
    component->type = POINT_LIGHT_T;
    component->local_id = _NPOINT_LIGHTS++;
    component->global_id = SCENE_N_COMPONENTS++;

    return component->global_id;
}
