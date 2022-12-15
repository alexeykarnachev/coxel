typedef struct PaneW {
    size_t rect;

    int is_hot;
} PaneW;

typedef struct ButtonW {
    size_t rect;
    size_t text;

    int is_pushed;
    int is_hot;
} ButtonW;


typedef struct InputW {
    size_t input_rect;
    size_t cursor_rect;
    size_t selection_rect;
    size_t label_text;
    size_t input_text;

    int is_selecting;
    int is_hot;
} InputW;

static PaneW PANES[256];
static ButtonW BUTTONS[128];
static InputW INPUTS[128];

static size_t N_PANES = 0;
static size_t N_BUTTONS = 0;
static size_t N_INPUTS = 0;


/*
ECS Primitives: rect, text, ...
*/
static size_t create_rect(
    int parent, int x, int y, size_t width, size_t height, Vec4 color
) {
    size_t rect = ecs_create_entity(parent);
    ecs_add_component(
        rect, GUI_RECT_T, gui_rect_create(width, height, color)
    );
    ecs_add_component(
        rect,
        TRANSFORMATION_T,
        transformation_create(vec3_ones, vec3_zeros, vec3(x, y, 0))
    );

    return rect;
}

static size_t create_text(
    int parent, char* label, int x, int y, size_t font_size, Vec3 color 
) {
    size_t text = ecs_create_entity(parent);
    ecs_add_component(
        text, GUI_TEXT_T, gui_text_create(label, color, font_size)
    );
    ecs_add_component(
        text,
        TRANSFORMATION_T,
        transformation_create(vec3_ones, vec3_zeros, vec3(x, y, 0.0))
    );

    return text;
}


/*
GUI widgets: pane, button, text input, ...
*/
static size_t create_pane(
    size_t x, size_t y, size_t width, size_t height, Vec4 color
) {
    size_t rect = create_rect(-1, x, y, width, height, color);

    PaneW* pane = &PANES[N_PANES++];
    pane->rect = rect;
    pane->is_hot = 0;

    return pane;
}

static ButtonW* create_button(
    int parent,
    char* text,
    size_t x,
    size_t y,
    size_t width,
    size_t height,
    size_t font_size,
    Vec4 rect_cold_color,
    Vec4 text_cold_color,
     
) {
    float char_width = GUI_FONT_ASPECT * (float)font_size;
    size_t text_width = strlen(label) * char_width;

    ButtonW* button = &BUTTONS[N_BUTTONS++];
    button->rect = create_rect(parent, x, y, width, height, cold_color);
    button->text = create_text(
        text,
        label,
        (width - text_width) / 2,
        (height - font_size) / 2,
        font_size,
        text_cold_color
    );
    button->is_pushed = 0;
    button->is_hot = 0;

    return button;
}

static InputW* create_input(
    int parent,
    char* label_text,
    size_t x,
    size_t y,
    size_t width,
    size_t border_width,
    size_t cursor_width,
    size_t font_size,
    Vec4 text_color,
    Vec4 selection_color,
    Vec4 cursor_color
) {
    float input_char_width = GUI_FONT_ASPECT * (float)INPUT_FONT_SIZE;
    size_t input_rect_hight = font_size + border_width * 2;

    size_t input_rect = create_rect(
        parent,
        x,
        y,
        width,
        input_rect_hight,
        text_color
    );
    size_t label_text = create_text(
        input_rect,
        label,
        -strlen(label) * input_char_width,
        (input_rect_hight - font_size) / 2,
        font_size,
        text_color
    );
    size_t input_text = create_text(
        input_rect,
        "",
        border_width,
        (input_rect_hight - font_size) / 2,
        font_size,
        text_color
    );
    size_t selection_rect = create_rect(
        input_text,
        0,
        (input_rect_hight - font_size) / 2,
        0,
        font_size,
        selection_color
    );
    size_t cursor_rect = create_rect(
        input_text,
        0,
        (input_rect_hight - input_font_size) / 2,
        cursor_width,
        input_font_size,
        cursor_color
    );

    ecs_disable_component(cursor_rect, GUI_RECT_T);

    InputW* input = &INPUTS[N_INPUTS++];
    input->input_rect = input_rect;
    input->cursor_rect = cursor_rect;
    input->selection_rect = selection_rect;
    input->label_text = label_text;
    input->input_text = input_text;
    input->is_selecting = 0;
    input->is_hot = 0;

    return input;
}
