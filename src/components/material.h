#pragma once
#include "../la.h"

typedef struct Material {
    Vec3 diffuse_color;
    float specular;
} Material;

Material* material_create(Vec3 diffuse_color, float specular);
Material* material_create_default();
