void editor_gui_create() {
    // Left pane
    size_t pane = ecs_create_entity(-1);
    ecs_add_component(pane, GUI_RECT_T, gui_rect_create(200, 200, vec4(0.2, 0.2, 0.2, 0.8)));
    ecs_add_component(
        pane, TRANSFORMATION_T,
        transformation_create(
            vec3_ones,
            vec3_zeros,
            vec3(10, 10, 0)
        ) 
    );
    size_t title = ecs_create_entity(pane);
    ecs_add_component(
        title, GUI_TEXT_T, gui_text_create("COXEL v0.0.1", 18));
    ecs_add_component(
        title, TRANSFORMATION_T,
        transformation_create(
            vec3_ones,
            vec3_zeros,
            vec3(10.0, 2.0, 0.0)
        ) 
    );

    size_t button = ecs_create_entity(pane);
    ecs_add_component(button, GUI_RECT_T, gui_rect_create(180, 20, vec4(0.4, 0.4, 0.4, 1.0)));
    ecs_add_component(
        button, TRANSFORMATION_T,
        transformation_create(
            vec3_ones,
            vec3_zeros,
            vec3(10, 10, 0)
        ) 
    );

}

void editor_gui_update() {
    for (size_t i = 0; i < N_GUI_BUTTON_ENTITIES; ++i) {
        size_t entity = GUI_BUTTON_ENTITIES[i];

    } 
}
