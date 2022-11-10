void entity_create_gui_pane(
    float x,
    float y,
    float width,
    float height,
    char* title_text
) {
    size_t pane = ecs_create_entity();
    ecs_add_component(pane, GUI_RECT_T, gui_rect_create());
    ecs_add_component(
        pane, TRANSFORMATION_T,
        transformation_create(
            vec3(width, height, 0.0),
            vec3_zeros(),
            vec3(x, y, 0.0)
        ) 
    );

    size_t title = ecs_create_entity();
    ecs_add_component(title, GUI_TEXT_T, gui_text_create(title_text));
    ecs_add_component(
        title, TRANSFORMATION_T,
        transformation_create(
            vec3(0.008, 0.018, 0.0),
            vec3_zeros(),
            vec3(x + 0.003, y + height - 0.018 - 0.003, 0.0)
        ) 
    );
}
