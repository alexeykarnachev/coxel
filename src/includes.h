#define PI 3.14159265358979323846264338327950288
#define deg2rad(d) ((d) * PI / 180.0)
#define rad2deg(r) ((r) * 180.0 / PI)

const char* VERT_MODEL_SPACE = "./shaders/space/model.vert";
const char* VERT_PROJ_SPACE = "./shaders/space/proj.vert";
const char* VERT_PLANE = "./shaders/space/plane.vert";
const char* TESC_TRIANGLES = "./shaders/tessellation/triangles.tesc";
const char* TESE_PERLIN_TRIANGLES = "./shaders/tessellation/perlin_triangles.tese";
const char* FRAG_BLINN_PHONG = "./shaders/lighting/blinn_phong.frag";
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
#include "gl_utils.h"
#include "la.h"
#include "camera.h"
#include "meshes.h"
#include "hdr.h"
#include "model.h"
