#define PI 3.14159265358979323846264338327950288
#define deg2rad(d) ((d) * PI / 180.0)
#define rad2deg(r) ((r) * 180.0 / PI)
#define clear_struct(s) (memset(s, 0, sizeof(*s)))

const char* VERT_MODEL_SPACE = "./shaders/space/model.vert";
const char* VERT_WORLD_SPACE = "./shaders/space/world.vert";
const char* VERT_PROJ_SPACE = "./shaders/space/proj.vert";
const char* VERT_PLANE = "./shaders/space/plane.vert";
const char* TESC_TRIANGLE = "./shaders/tessellation/triangle.tesc";
const char* TESE_PERLIN_TRIANGLES = "./shaders/tessellation/perlin_triangles.tese";
const char* GEOM_POINT_SHADOWS = "./shaders/shadows/point_shadows.geom";
const char* FRAG_BLINN_PHONG = "./shaders/lighting/blinn_phong.frag";
const char* FRAG_SHADOWS_DEPTH = "./shaders/shadows/shadows_depth.frag";
const char* FRAG_EMPTY = "./shaders/common/empty.frag";
const char* GLSL_RANDOM = "./shaders/common/random.glsl";


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

#include "utils.h"
#include "la.h"
#include "renderer/camera.h"
#include "renderer/material.h"
#include "renderer/texture.h"
#include "renderer/buffer.h"
#include "renderer/mesh.h"
#include "renderer/light.h"
#include "renderer/program.h"
#include "renderer/transformation.h"
#include "renderer/renderer.h"
