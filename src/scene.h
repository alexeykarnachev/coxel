size_t _SCENE_CREATED = false;

int32_t SCENE_ACTIVE_CAMERA_ID = -1;

size_t SCENE_N_MESHES = 0;
size_t SCENE_N_CAMERAS = 0;
size_t SCENE_N_MATERIALS = 0;
size_t SCENE_N_POINT_LIGHTS = 0;
size_t SCENE_N_POINT_SHADOW_CASTERS = 0;
size_t SCENE_N_SCRIPTS = 0;
size_t SCENE_N_GUI_PANES = 0;
size_t SCENE_N_GUI_TEXTS = 0;

Mesh SCENE_MESHES[MAX_N_MESHES];
Camera SCENE_CAMERAS[MAX_N_CAMERAS];
Material SCENE_MATERIALS[MAX_N_MATERIALS];
PointLight SCENE_POINT_LIGHTS[MAX_N_POINT_LIGHTS];
PointShadowCaster SCENE_POINT_SHADOW_CASTERS[MAX_N_POINT_SHADOW_CASTERS];
GUIPane SCENE_GUI_PANES[MAX_N_GUI_PANES];
GUIText SCENE_GUI_TEXTS[MAX_N_GUI_TEXTS];
PointLight SCENE_GUI_POINT_LIGHTS[MAX_N_POINT_LIGHTS];
PointShadowCaster SCENE_GUI_POINT_SHADOW_CASTERS[MAX_N_POINT_SHADOW_CASTERS];
Script SCENE_SCRIPTS[MAX_N_SCRIPTS];

DepthCubemapArray SCENE_POINT_SHADOW_BUFFER;
DeferredBuffer SCENE_DEFERRED_BUFFER;
GLuint SCENE_GUI_FONT_TEXTURE;

UBOStructsArray SCENE_CAMERA_BUFFERS;
UBOStructsArray SCENE_MATERIAL_BUFFERS;
UBOStructsArray SCENE_POINT_LIGHT_BUFFERS;
UBOStructsArray SCENE_POINT_SHADOW_CASTER_BUFFERS;
UBOStructsArray SCENE_GUI_TEXT_BUFFERS;


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
        &SCENE_GUI_TEXT_BUFFERS,
        MAX_N_GUI_TEXTS,
        GUI_TEXT_PACK_SIZE,
        GUI_TEXT_BINDING_IDX,
        GL_STATIC_DRAW
    );

    ok &= depth_cubemap_array_create(
        &SCENE_POINT_SHADOW_BUFFER, POINT_SHADOW_SIZE, MAX_N_POINT_SHADOW_CASTERS);
    ok &= texture_create_1d(
        &SCENE_GUI_FONT_TEXTURE, GUI_FONT_RASTER, 0, GUI_FONT_TEXTURE_WIDTH,
        GL_R8, GL_RED, GL_UNSIGNED_BYTE);
    ok &= deferred_buffer_create(&SCENE_DEFERRED_BUFFER, DEFERRED_BUFFER_WIDTH, DEFERRED_BUFFER_HEIGHT);

    _SCENE_CREATED = ok;
    return ok;
}

int scene_add_mesh(Mesh mesh) {
    SCENE_MESHES[SCENE_N_MESHES] = mesh;
    return SCENE_N_MESHES++;
}

int scene_add_camera(Camera camera) {
    static float pack[CAMERA_PACK_SIZE];
    camera_pack(&camera, pack);
    ubo_structs_array_add(&SCENE_CAMERA_BUFFERS, pack);
    SCENE_CAMERAS[SCENE_N_CAMERAS] = camera;
    return SCENE_N_CAMERAS++;
}

int scene_add_material(Material material) {
    static float pack[MATERIAL_PACK_SIZE];
    material_pack(&material, pack);
    ubo_structs_array_add(&SCENE_MATERIAL_BUFFERS, pack);
    SCENE_MATERIALS[SCENE_N_MATERIALS] = material;
    return SCENE_N_MATERIALS++;
}

int scene_add_point_light(PointLight point_light) {
    static float pack[POINT_LIGHT_PACK_SIZE];
    point_light_pack(&point_light, pack);
    ubo_structs_array_add(&SCENE_POINT_LIGHT_BUFFERS, pack);
    SCENE_POINT_LIGHTS[SCENE_N_POINT_LIGHTS] = point_light;
    return SCENE_N_POINT_LIGHTS++;
}

int scene_add_point_shadow_caster(PointShadowCaster point_shadow_caster) {
    static float pack[POINT_SHADOW_CASTER_PACK_SIZE];
    point_shadow_caster_pack(&point_shadow_caster, pack);
    ubo_structs_array_add(&SCENE_POINT_SHADOW_CASTER_BUFFERS, pack);
    SCENE_POINT_SHADOW_CASTERS[SCENE_N_POINT_SHADOW_CASTERS] = point_shadow_caster;
    return SCENE_N_POINT_SHADOW_CASTERS++;
}

int scene_add_gui_pane(GUIPane gui_pane) {
    SCENE_GUI_PANES[SCENE_N_GUI_PANES] = gui_pane;
    return SCENE_N_GUI_PANES++;
}

int scene_add_gui_text(GUIText gui_text) {
    static float pack[GUI_TEXT_PACK_SIZE];
    gui_text_pack(&gui_text, pack);
    ubo_structs_array_add(&SCENE_GUI_TEXT_BUFFERS, pack);
    SCENE_GUI_TEXTS[SCENE_N_GUI_TEXTS] = gui_text;
    return SCENE_N_GUI_TEXTS++;
}

int scene_add_script(Script script) {
    SCENE_SCRIPTS[SCENE_N_SCRIPTS] = script;
    return SCENE_N_SCRIPTS++;
}

void scene_update_camera(size_t id) {
    static float pack[CAMERA_PACK_SIZE];
    camera_pack(&SCENE_CAMERAS[id], pack);
    ubo_structs_array_update(&SCENE_CAMERA_BUFFERS, pack, id);
}

