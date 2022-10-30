#define PI 3.14159265358979323846264338327950288
#define EPS 0.00001

#define deg2rad(d) ((d) * PI / 180.0)
#define rad2deg(r) ((r) * 180.0 / PI)
#define clear_struct(s) (memset(s, 0, sizeof(*s)))
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
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
#include "utils.h"
#include "la.h"
#include "font.h"
#include "program.h"
#include "texture.h"
#include "primitives.h"
#include "components/camera.h"
#include "components/material.h"
#include "components/transformation.h"
#include "components/mesh.h"
#include "components/point_shadow_caster.h"
#include "components/point_light.h"
#include "components/script.h"
#include "components/gui_pane.h"
#include "components/gui_text.h"
#include "buffers/array_buffer.h"
#include "buffers/depth_cubemap_array.h"
#include "buffers/ubo_structs_array.h"
#include "window.h"
#include "scene.h"
#include "scripts/camera_mouse_controller.h"
#include "renderer.h"

