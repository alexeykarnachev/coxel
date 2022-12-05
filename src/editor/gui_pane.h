void entity_create_gui_pane(
    size_t x,
    size_t y,
    size_t width,
    size_t height,
    size_t font_height,
    char* title_text
) {
    size_t pane = ecs_create_entity(-1);
    ecs_add_component(pane, GUI_RECT_T, gui_rect_create(width, height));
    ecs_add_component(
        pane, TRANSFORMATION_T,
        transformation_create(
            vec3_ones,
            vec3_zeros,
            vec3(x, y, 0.0)
        ) 
    );

    size_t title = ecs_create_entity(pane);
    ecs_add_component(
        title, GUI_TEXT_T, gui_text_create(title_text, font_height));
    ecs_add_component(
        title, TRANSFORMATION_T,
        transformation_create(
            vec3_ones,
            vec3_zeros,
            vec3(10.0, 0.0, 0.0)
        ) 
    );
}
