#pragma once
#include <math.h>
#include <stdio.h>
#include <string.h>

#define PI 3.14159265358979323846264338327950288
#define EPS 0.0000001

#define deg2rad(d) ((d)*PI / 180.0)
#define rad2deg(r) ((r)*180.0 / PI)
#define clear_struct(s) (memset(s, 0, sizeof(*s)))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define sign(x) ((fabs(x) > EPS ? (x > 0.0 ? 1.0 : -1.0) : 0.0))
#define check_framebuffer(fail_return_value) \
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) \
        != GL_FRAMEBUFFER_COMPLETE) { \
        fprintf(stderr, "ERROR: framebuffer not complete\n"); \
        return fail_return_value; \
    }
