#define POINT_LIGHT_PACK_SIZE 48

typedef struct PointLight {
    Vec3 world_pos;
    Vec3 color;
    float energy;
} PointLight;


void point_light_pack(PointLight* point_light, float dst[]) {
    memcpy(&dst[0], point_light->world_pos.data, 12);
    memcpy(&dst[4], point_light->color.data, 12);
    dst[8] = point_light->energy;
}

