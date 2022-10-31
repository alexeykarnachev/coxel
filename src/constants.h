#define MAX_N_CAMERAS 16
#define MAX_N_MESHES 512
#define MAX_N_TRANSFORMATIONS 512
#define MAX_N_MATERIALS 256
#define MAX_N_POINT_LIGHTS 128
#define MAX_N_POINT_SHADOW_CASTERS 16
#define MAX_N_SCRIPTS 512
#define MAX_N_GUI_PANES 16
#define MAX_N_GUI_TEXTS 32
#define MAX_N_SCENE_COMPONENTS MAX_N_CAMERAS + MAX_N_MESHES + MAX_N_TRANSFORMATIONS + MAX_N_MATERIALS + MAX_N_POINT_LIGHTS + MAX_N_POINT_SHADOW_CASTERS + MAX_N_SCRIPTS + MAX_N_GUI_PANES + MAX_N_GUI_TEXTS

#define GUI_TEXT_MAX_N_CHARS 32

#define CAMERA_BINDING_IDX 0
#define MATERIAL_BINDING_IDX 1
#define POINT_LIGHT_BINDING_IDX 2
#define POINT_SHADOW_CASTER_BINDING_IDX 3
#define TRANSFORMATION_BINDING_IDX 4
#define GUI_PANE_BINDING_IDX 5
#define GUI_TEXT_BINDING_IDX 6

#define POINT_SHADOW_TEXTURE_LOCATION_IDX 0
#define GUI_FONT_TEXTURE_LOCATION_IDX 1

#define POINT_SHADOW_SIZE 1024
#define POINT_SHADOW_NEAR_PLANE 0.1
#define POINT_SHADOW_FAR_PLANE 100
#define POINT_SHADOW_MIN_N_SAMPLES 8
#define POINT_SHADOW_MAX_N_SAMPLES 64
#define POINT_SHADOW_DISK_RADIUS 0.01
#define POINT_SHADOW_BIAS_MIN 0.001
#define POINT_SHADOW_BIAS_MAX 0.01
