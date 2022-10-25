#define POINT_LIGHT_PACK_SIZE 48

typedef struct PointLight {
    Vec3 world_pos;
    Vec3 color;
    float energy;
} PointLight;


PointLight point_light_create(Vec3 world_pos, Vec3 color, float energy) {
    PointLight point_light;

    point_light.world_pos = world_pos;
    point_light.color = color;
    point_light.energy = energy;

    return point_light;
}

void point_light_pack(PointLight* point_light, float dst[]) {
    size_t size = sizeof(float);
    memcpy(&dst[0], point_light->world_pos.data, size * 3);
    memcpy(&dst[4], point_light->color.data, size * 3);
    dst[8] = point_light->energy;
}

