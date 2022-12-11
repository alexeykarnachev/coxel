#pragma once
#include "../defaults.h"
#include "../la.h"

typedef struct PointLight {
    Vec3 color;
    float linear;
    float quadratic;
} PointLight;

PointLight* point_light_create(Vec3 color, float linear, float quadratic);
PointLight* point_light_create_default();
