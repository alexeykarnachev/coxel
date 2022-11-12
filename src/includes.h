#define PI 3.14159265358979323846264338327950288
#define EPS 0.0000001

#define deg2rad(d) ((d) * PI / 180.0)
#define rad2deg(r) ((r) * 180.0 / PI)
#define clear_struct(s) (memset(s, 0, sizeof(*s)))
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#define sign(x) ((fabs(x) > EPS ? (x > 0 ? 1 : -1) : 0))
#define check_framebuffer(fail_return_value) \
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {\
        fprintf(stderr, "ERROR: framebuffer not complete\n");\
        return fail_return_value;\
    }\

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "constants.h"
#include "defaults.h"
#include "utils.h"
#include "la.h"
#include "font.h"
#include "program.h"
#include "texture.h"
#include "primitives.h"
#include "bitset.h"
#include "assets/sun_icon_32.h"
#include "buffers/vao_buffer.h"
#include "buffers/texture_buffer.h"
#include "buffers/depth_cubemap_array.h"
#include "components/transformation.h"
#include "components/camera.h"
#include "components/material.h"
#include "components/mesh.h"
#include "components/sprite.h"
#include "components/point_shadow_caster.h"
#include "components/point_light.h"
#include "components/script.h"
#include "ecs.h"
#include "editor/gui_rect.h"
#include "editor/gui_text.h"
#include "editor/gui_pane.h"
#include "window.h"
#include "scripts/camera_mouse_controller.h"
#include "scripts/entity_mouse_selection.h"
#include "scripts/entity_mouse_drag.h"
#include "renderer.h"

