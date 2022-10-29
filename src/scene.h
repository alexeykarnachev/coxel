typedef enum {
    ERROR_T,
    MESH_T,
    CAMERA_T,
    MATERIAL_T,
    TRANSFORMATION_T,
    POINT_LIGHT_T,
    POINT_SHADOW_CASTER_T,
    SCRIPT_T,
    GUI_PANE_T,
    GUI_TEXT_T
} ComponentType;

typedef struct SceneComponent {
    ComponentType type;
    size_t gid;
    size_t lid;
    void* ptr;
} SceneComponent;

size_t SCENE_N_COMPONENTS = 0;
size_t _SCENE_CREATED = false;
int32_t _SCENE_ACTIVE_CAMERA_GID = -1;
size_t _N_MESHES = 0;
size_t _N_CAMERAS = 0;
size_t _N_MATERIALS= 0;
size_t _N_TRANSFORMATIONS = 0;
size_t _N_POINT_LIGHTS = 0;
size_t _N_POINT_SHADOW_CASTERS = 0;
size_t _N_SCRIPTS = 0;
size_t _N_GUI_PANES = 0;
size_t _N_GUI_TEXTS = 0;
size_t _N_FONT_GLYPHS = 0;

SceneComponent SCENE_COMPONENTS[MAX_N_SCENE_COMPONENTS];
ArrayBuffer SCENE_MESH_BUFFERS[MAX_N_MESHES];
DepthCubemapArray SCENE_POINT_SHADOW_BUFFER;
Texture SCENE_GUI_FONT_TEXTURE;
UBOStructsArray SCENE_CAMERA_BUFFERS;
UBOStructsArray SCENE_MATERIAL_BUFFERS;
UBOStructsArray SCENE_TRANSFORMATION_BUFFERS;
UBOStructsArray SCENE_POINT_LIGHT_BUFFERS;
UBOStructsArray SCENE_POINT_SHADOW_CASTER_BUFFERS;
UBOStructsArray SCENE_GUI_PANE_BUFFERS;
UBOStructsArray SCENE_GUI_TEXT_BUFFERS;
UBOStructsArray SCENE_FONT_GLYPH_BUFFERS;
UBOStructsArray SCENE_GUI_FONT_BUFFERS;



#define _check_scene(ptr_, curr_n, max_n)\
    if (!_SCENE_CREATED) {\
        fprintf(stderr, "ERROR: can't add " #ptr_ ". Create a scene first\n");\
        return -1;\
    }\
    if (curr_n == max_n || SCENE_N_COMPONENTS == MAX_N_SCENE_COMPONENTS) {\
        fprintf(stderr, "ERROR: can't add " #ptr_ ". The maximum is reached\n");\
        return -1;\
    }\

#define _add_component(ptr_, type_, curr_n)\
    SceneComponent* component = &SCENE_COMPONENTS[SCENE_N_COMPONENTS];\
    component->type = type_;\
    component->lid = curr_n++;\
    component->gid = SCENE_N_COMPONENTS++;\
    component->ptr = ptr_;\

#define _add_ubo_component(ptr_, curr_n, max_n, pack_size, buffers, type_, pack_fn)\
    static float pack[pack_size];\
    pack_fn(ptr_, pack);\
    ubo_structs_array_add(&buffers, pack);\
    _add_component(ptr_, type_, curr_n)\

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
    ok &= ubo_structs_array_create(
        &SCENE_POINT_SHADOW_CASTER_BUFFERS,
        MAX_N_POINT_SHADOW_CASTERS,
        POINT_SHADOW_CASTER_PACK_SIZE,
        POINT_SHADOW_CASTER_BINDING_IDX,
        GL_STATIC_DRAW
    );
    ok &= ubo_structs_array_create(
        &SCENE_GUI_PANE_BUFFERS,
        MAX_N_GUI_PANES,
        GUI_PANE_PACK_SIZE,
        GUI_PANE_BINDING_IDX,
        GL_STATIC_DRAW
    );
    ok &= ubo_structs_array_create(
        &SCENE_GUI_TEXT_BUFFERS,
        MAX_N_GUI_TEXTS,
        GUI_TEXT_PACK_SIZE,
        GUI_TEXT_BINDING_IDX,
        GL_STATIC_DRAW
    );
    ok &= ubo_structs_array_create(
        &SCENE_GUI_FONT_BUFFERS,
        1,
        GUI_FONT_PACK_SIZE,
        GUI_FONT_BINDING_IDX,
        GL_STATIC_DRAW
    );

    ok &= depth_cubemap_array_create(
        &SCENE_POINT_SHADOW_BUFFER, POINT_SHADOW_SIZE, MAX_N_POINT_SHADOW_CASTERS);
    ok &= texture_create(
        &SCENE_GUI_FONT_TEXTURE, GUI_FONT_TEXTURE_WIDTH, GUI_FONT_TEXTURE_HEIGHT, 1, 4);

    _SCENE_CREATED = ok;
    return ok;
}

bool scene_set_active_camera_gid(size_t gid) {
    SceneComponent* component = &SCENE_COMPONENTS[gid];
    if (component->type != CAMERA_T) {
        fprintf(stderr, "ERROR: can't set scene camera. Passed gid doesn't belong to a camera");
        return false;
    }
    _SCENE_ACTIVE_CAMERA_GID = gid;
    return true;
}

bool scene_set_gui_font(GUIFont* gui_font) {
    float pack[GUI_FONT_PACK_SIZE];
    gui_font_pack(gui_font, pack);
    ubo_structs_array_add(&SCENE_GUI_FONT_BUFFERS, pack);
    return texture_set_sprite(&SCENE_GUI_FONT_TEXTURE, &gui_font->sprite, 0);
}

int32_t scene_get_active_camera_lid() {
    if (_SCENE_ACTIVE_CAMERA_GID == -1) {
        return -1;
    }

    SceneComponent* component = &SCENE_COMPONENTS[_SCENE_ACTIVE_CAMERA_GID];
    return component->lid;
}

int scene_add_mesh(Mesh* mesh) {
    _check_scene(mesh, _N_MESHES, MAX_N_MESHES)

    ArrayBuffer* b = &SCENE_MESH_BUFFERS[_N_MESHES];
    bool ok = array_buffer_create(
         b, mesh->faces, mesh->faces_size, mesh->vertices, mesh->vertices_size, GL_STATIC_DRAW);
    if (!ok) {
        fprintf(stderr, "ERROR: can't add mesh. Check array buffer creation error\n");
        return -1;
    }

    _add_component(mesh, MESH_T, _N_MESHES)
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
    return component->gid;
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
    return component->gid;
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
    return component->gid;
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
    return component->gid;
}

int scene_add_point_shadow_caster(PointShadowCaster* point_shadow_caster) {
    _add_ubo_component(
        point_shadow_caster,
        _N_POINT_SHADOW_CASTERS,
        MAX_N_POINT_SHADOW_CASTERS,
        POINT_SHADOW_CASTER_PACK_SIZE,
        SCENE_POINT_SHADOW_CASTER_BUFFERS,
        POINT_SHADOW_CASTER_T,
        point_shadow_caster_pack
    )
    return component->gid;
}

int scene_add_gui_pane(GUIPane* gui_pane) {
    _add_ubo_component(
        gui_pane,
        _N_GUI_PANES,
        MAX_N_GUI_PANES,
        GUI_PANE_PACK_SIZE,
        SCENE_GUI_PANE_BUFFERS,
        GUI_PANE_T,
        gui_pane_pack
    )
    return component->gid;
}

int scene_add_script(Script* script) {
    _check_scene(script, _N_SCRIPTS, MAX_N_SCRIPTS)
    _add_component(script, SCRIPT_T, _N_SCRIPTS)
    return component->gid;
}

void scene_update_component(int32_t gid) {
    SceneComponent* component = &SCENE_COMPONENTS[gid];
    int32_t lid = component->lid;
    void* ptr = component->ptr;
    
    switch (component->type) {
        case CAMERA_T: ;
            static float pack[CAMERA_PACK_SIZE];
            camera_pack((Camera*)ptr, pack);
            ubo_structs_array_update(&SCENE_CAMERA_BUFFERS, pack, lid);
            break;
        default:
            fprintf(
                stderr,
                "ERROR: can't update component of type %d. " \
                "Update logic is not implemented",
                component->type
            );
            break;
    }
}