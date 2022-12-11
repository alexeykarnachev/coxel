#include "point_light.h"

static PointLight POINT_LIGHTS_ARENA[1 << 14];
static size_t N_POINT_LIGHTS = 0;

PointLight* point_light_create(Vec3 color, float linear, float quadratic) {
    PointLight* p = &POINT_LIGHTS_ARENA[N_POINT_LIGHTS++];

    p->color = color;
    p->linear = linear;
    p->quadratic = quadratic;

    return p;
}

PointLight* point_light_create_default() {
    return point_light_create(
        DEFAULT_POINT_LIGHT_COLOR,
        DEFAULT_POINT_LIGHT_LINEAR,
        DEFAULT_POINT_LIGHT_QUADRATIC
    );
}
