typedef struct Scene {
    int32_t active_camera_id;

    size_t n_meshes;
    size_t n_cameras;
    size_t n_materials;
    size_t n_point_lights;
    size_t n_point_shadow_casters;
    size_t n_scripts;
    size_t n_gui_panes;
    size_t n_gui_texts;

    Mesh meshes[MAX_N_MESHES];
    Camera cameras[MAX_N_CAMERAS];
    Material materials[MAX_N_MATERIALS];
    PointLight point_lights[MAX_N_POINT_LIGHTS];
    PointShadowCaster point_shadow_casters[MAX_N_POINT_SHADOW_CASTERS];
    GUIPane gui_panes[MAX_N_GUI_PANES];
    GUIText gui_texts[MAX_N_GUI_TEXTS];
    PointLight gui_point_lights[MAX_N_POINT_LIGHTS];
    PointShadowCaster gui_point_shadow_casters[MAX_N_POINT_SHADOW_CASTERS];
    Script scripts[MAX_N_SCRIPTS];

    DepthCubemapArray point_shadow_buffer;
    GBuffer gbuffer;
    GLuint gui_font_texture;

    UBOStructsArray camera_buffers;
    UBOStructsArray material_buffers;
    UBOStructsArray point_light_buffers;
    UBOStructsArray point_shadow_caster_buffers;
    UBOStructsArray gui_text_buffers;

    int32_t is_created;
} Scene;

Scene SCENE;

bool scene_create() {
    clear_struct(&SCENE);

    bool ok = true;
    ok &= ubo_structs_array_create(
        &SCENE.camera_buffers,
        MAX_N_CAMERAS,
        CAMERA_PACK_SIZE,
        CAMERA_BINDING_IDX,
        GL_DYNAMIC_DRAW
    );
    ok &= ubo_structs_array_create(
        &SCENE.material_buffers,
        MAX_N_MATERIALS,
        MATERIAL_PACK_SIZE,
        MATERIAL_BINDING_IDX,
        GL_STATIC_DRAW
    );
    ok &= ubo_structs_array_create(
        &SCENE.point_light_buffers,
        MAX_N_POINT_LIGHTS,
        POINT_LIGHT_PACK_SIZE,
        POINT_LIGHT_BINDING_IDX,
        GL_STATIC_DRAW
    );
    ok &= ubo_structs_array_create(
        &SCENE.point_shadow_caster_buffers,
        MAX_N_POINT_SHADOW_CASTERS,
        POINT_SHADOW_CASTER_PACK_SIZE,
        POINT_SHADOW_CASTER_BINDING_IDX,
        GL_STATIC_DRAW
    );
    ok &= ubo_structs_array_create(
        &SCENE.gui_text_buffers,
        MAX_N_GUI_TEXTS,
        GUI_TEXT_PACK_SIZE,
        GUI_TEXT_BINDING_IDX,
        GL_STATIC_DRAW
    );

    ok &= depth_cubemap_array_create(
        &SCENE.point_shadow_buffer, POINT_SHADOW_SIZE, MAX_N_POINT_SHADOW_CASTERS);
    ok &= texture_create_1d(
        &SCENE.gui_font_texture, GUI_FONT_RASTER, 0, GUI_FONT_TEXTURE_WIDTH,
        GL_R8, GL_RED, GL_UNSIGNED_BYTE);
    ok &= gbuffer_create(&SCENE.gbuffer, GBUFFER_WIDTH, GBUFFER_HEIGHT);

    SCENE.is_created = ok;
    SCENE.active_camera_id = -1;

    return ok;
}

int scene_add_mesh(Mesh mesh) {
    SCENE.meshes[SCENE.n_meshes] = mesh;
    return SCENE.n_meshes++;
}

int scene_add_camera(Camera camera) {
    static float pack[CAMERA_PACK_SIZE];
    camera_pack(&camera, pack);
    ubo_structs_array_add(&SCENE.camera_buffers, pack);
    SCENE.cameras[SCENE.n_cameras] = camera;
    return SCENE.n_cameras++;
}

int scene_add_material(Material material) {
    static float pack[MATERIAL_PACK_SIZE];
    material_pack(&material, pack);
    ubo_structs_array_add(&SCENE.material_buffers, pack);
    SCENE.materials[SCENE.n_materials] = material;
    return SCENE.n_materials++;
}

int scene_add_point_light(PointLight point_light) {
    static float pack[POINT_LIGHT_PACK_SIZE];
    point_light_pack(&point_light, pack);
    ubo_structs_array_add(&SCENE.point_light_buffers, pack);
    SCENE.point_lights[SCENE.n_point_lights] = point_light;
    return SCENE.n_point_lights++;
}

int scene_add_point_shadow_caster(PointShadowCaster point_shadow_caster) {
    static float pack[POINT_SHADOW_CASTER_PACK_SIZE];
    point_shadow_caster_pack(&point_shadow_caster, pack);
    ubo_structs_array_add(&SCENE.point_shadow_caster_buffers, pack);
    SCENE.point_shadow_casters[SCENE.n_point_shadow_casters] = point_shadow_caster;
    return SCENE.n_point_shadow_casters++;
}

int scene_add_gui_pane(GUIPane gui_pane) {
    SCENE.gui_panes[SCENE.n_gui_panes] = gui_pane;
    return SCENE.n_gui_panes++;
}

int scene_add_gui_text(GUIText gui_text) {
    static float pack[GUI_TEXT_PACK_SIZE];
    gui_text_pack(&gui_text, pack);
    ubo_structs_array_add(&SCENE.gui_text_buffers, pack);
    SCENE.gui_texts[SCENE.n_gui_texts] = gui_text;
    return SCENE.n_gui_texts++;
}

int scene_add_script(Script script) {
    SCENE.scripts[SCENE.n_scripts] = script;
    return SCENE.n_scripts++;
}

void scene_update_camera(size_t id) {
    static float pack[CAMERA_PACK_SIZE];
    camera_pack(&SCENE.cameras[id], pack);
    ubo_structs_array_update(&SCENE.camera_buffers, pack, id);
}

void scene_update_material(size_t id) {
    static float pack[MATERIAL_PACK_SIZE];
    material_pack(&SCENE.materials[id], pack);
    ubo_structs_array_update(&SCENE.material_buffers, pack, id);
}

void scene_update_mesh(size_t id) {
    size_t material_id = SCENE.meshes[id].material_id;
    scene_update_material(material_id);
}

