typedef struct PointLight {
    Vec3 color;
    float energy;
} PointLight;

PointLight _POINT_LIGHTS_ARENA[1 << 14];
size_t _N_POINT_LIGHTS = 0;


PointLight* point_light_create(Vec3 color, float energy) {
    PointLight* p = &_POINT_LIGHTS_ARENA[_N_POINT_LIGHTS++];

    p->color = color;
    p->energy = energy;

    return p;
}

PointLight* point_light_create_default() {
    return point_light_create(vec3_ones(), 50.0);
}
